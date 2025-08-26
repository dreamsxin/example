import flet as ft
import pytz
import os
import requests
import json
import pproxy
import asyncio
import geoip2.database
from functools import lru_cache
from timezonefinder import TimezoneFinder
from playwright.async_api import async_playwright
from playwright.async_api._generated import BrowserContext

# 常量定义
COUNTRY_DATABASE_PATH = "GeoLite2-Country.mmdb"
CITY_DATABASE_PATH = "GeoLite2-City.mmdb"

# 屏幕分辨率选项
SCREENS = ("800×600", "960×540", "1024×768", "1152×864", "1280×720", "1280×768", "1280×800", 
           "1280×1024", "1366×768", "1408×792", "1440×900", "1400×1050", "1440×1080", "1536×864", 
           "1600×900", "1600×1024", "1600×1200", "1680×1050", "1920×1080", "1920×1200", 
           "2048×1152", "2560×1080", "2560×1440", "3440×1440")

# 语言选项
LANGUAGES = ("en-US", "en-GB", "fr-FR", "ru-RU", "es-ES", "pl-PL", "pt-PT", "nl-NL", "zh-CN")

# 时区选项
TIMEZONES = pytz.common_timezones

# 获取最新用户代理
try:
    USER_AGENT = requests.get(
        "https://raw.githubusercontent.com/microlinkhq/top-user-agents/refs/heads/master/src/index.json",
        timeout=10
    ).json()[0]
except (requests.RequestException, json.JSONDecodeError, IndexError):
    USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"

async def save_cookies(context: BrowserContext, profile: str) -> None:
    """保存cookies到文件"""
    cookies = await context.cookies()

    for cookie in cookies:
        cookie.pop("sameSite", None)

    os.makedirs("cookies", exist_ok=True)
    with open(f"cookies/{profile}", "w", encoding="utf-8") as f:
        json.dump(obj=cookies, fp=f, indent=4)

def parse_netscape_cookies(netscape_cookie_str: str) -> list[dict]:
    """解析Netscape格式的cookies"""
    cookies = []
    lines = netscape_cookie_str.strip().split("\n")

    for line in lines:
        if not line.startswith("#") and line.strip():
            parts = line.split('\t')
            if len(parts) >= 7:
                cookie = {
                    "domain": parts[0],
                    "httpOnly": parts[1].upper() == "TRUE",
                    "path": parts[2],
                    "secure": parts[3].upper() == "TRUE",
                    "expires": float(parts[4]),
                    "name": parts[5],
                    "value": parts[6]
                }
                cookies.append(cookie)
    
    return cookies

@lru_cache(maxsize=256)
def get_proxy_info(ip: str) -> dict:
    """获取代理IP的地理位置信息"""
    country_code = "未知"
    city = "未知"
    timezone = "未知"
    
    try:
        if os.path.exists(COUNTRY_DATABASE_PATH):
            with geoip2.database.Reader(COUNTRY_DATABASE_PATH) as reader:
                try:
                    response = reader.country(ip)
                    country_code = response.country.iso_code or "未知"
                except geoip2.errors.AddressNotFoundError:
                    country_code = "未知"
    except Exception:
        country_code = "未知"

    try:
        if os.path.exists(CITY_DATABASE_PATH):
            with geoip2.database.Reader(CITY_DATABASE_PATH) as reader:
                try:
                    response = reader.city(ip)
                    city = response.city.name or "未知"
                    if response.location.longitude and response.location.latitude:
                        timezone = TimezoneFinder().timezone_at(
                            lng=response.location.longitude, 
                            lat=response.location.latitude
                        ) or "未知"
                except geoip2.errors.AddressNotFoundError:
                    city = "未知"
    except Exception:
        city = "未知"
        timezone = "未知"

    return {"country_code": country_code, "city": city, "timezone": timezone}

async def run_proxy(protocol: str, ip: str, port: int, login: str, password: str):
    """运行代理服务器"""
    server = pproxy.Server("socks5://127.0.0.1:1337")
    remote = pproxy.Connection(f"{protocol}://{ip}:{port}#{login}:{password}")
    args = dict(rserver=[remote], verbose=print)
    
    await server.start_server(args)

async def run_browser(user_agent: str, height: int, width: int, timezone: str, lang: str, 
                     proxy: str | bool, cookies: dict | bool, webgl: bool, vendor: str, 
                     cpu: int, ram: int, is_touch: bool, profile: str) -> None:
    """运行浏览器实例"""
    protocol = None
    proxy_task = None
    
    try:
        async with async_playwright() as p:
            args = [
                "--no-sandbox",
                "--disable-setuid-sandbox",
                "--disable-web-security",
                "--ignore-certificate-errors",
                "--disable-infobars",
                "--disable-extensions",
                "--disable-blink-features=AutomationControlled",
            ]
            
            if not webgl:
                args.append("--disable-webgl")
            
            proxy_settings = None
            if proxy:
                protocol = proxy.split("://")[0]

                if "@" in proxy:
                    splitted = proxy.split("://")[1].split("@")
                    ip_port_part = splitted[1].split(":")
                    ip = ip_port_part[0]
                    port = int(ip_port_part[1])
                    auth_part = splitted[0].split(":")
                    username = auth_part[0]
                    password = auth_part[1] if len(auth_part) > 1 else ""
                else:
                    splitted = proxy.split("://")[1].split(":")
                    ip = splitted[0]
                    port = int(splitted[1])
                    username = splitted[2] if len(splitted) > 2 else ""
                    password = splitted[3] if len(splitted) > 3 else ""

                if protocol == "http":
                    proxy_settings = {
                        "server": f"{ip}:{port}",
                        "username": username,
                        "password": password
                    }
                else:
                    proxy_task = asyncio.create_task(run_proxy(protocol, ip, port, username, password))
                    proxy_settings = {
                        "server": "socks5://127.0.0.1:1337"
                    }

            launch_options = {"headless": False, "args": args}
            if proxy_settings:
                launch_options["proxy"] = proxy_settings
                
            browser = await p.chromium.launch(**launch_options)

            context = await browser.new_context(
                user_agent=user_agent,
                viewport={"width": width, "height": height},
                locale=lang,
                timezone_id=timezone,
                has_touch=is_touch
            )

            # 修改浏览器指纹
            await context.add_init_script(f"""
                Object.defineProperty(navigator, 'vendor', {{
                    get: function() {{
                        return '{vendor}';
                    }}
                }});
            """)

            await context.add_init_script(f"""
                Object.defineProperty(navigator, 'hardwareConcurrency', {{
                    get: function() {{
                        return {cpu};
                    }}
                }});
            """)

            await context.add_init_script(f"""
                Object.defineProperty(navigator, 'deviceMemory', {{
                    get: function() {{
                        return {ram};
                    }}
                }});
            """)

            # 处理cookies
            cookies_parsed = []
            cookies_file = f"cookies/{profile}"
            
            if cookies and os.path.isfile(cookies):
                # 从外部文件加载cookies
                with open(cookies, "r", encoding="utf-8") as f:
                    cookies_data = f.read()
                    try:
                        cookies_parsed = json.loads(cookies_data)
                    except json.decoder.JSONDecodeError:
                        cookies_parsed = parse_netscape_cookies(cookies_data)
            elif os.path.exists(cookies_file):
                # 加载已保存的cookies
                with open(cookies_file, "r", encoding="utf-8") as f:
                    try:
                        cookies_parsed = json.load(f)
                    except json.decoder.JSONDecodeError:
                        cookies_parsed = []
            else:
                cookies_parsed = []

            # 添加cookies到浏览器上下文
            if cookies_parsed:
                for cookie in cookies_parsed:
                    cookie["sameSite"] = "Strict"
                await context.add_cookies(cookies_parsed)
            
            page = await context.new_page()
            await page.evaluate("navigator.__proto__.webdriver = undefined;")
            await page.goto("about:blank")

            try:
                await page.wait_for_event("close", timeout=0)
            except Exception:
                pass  # 页面被正常关闭
            finally:
                await save_cookies(context, profile)
                await browser.close()
                
    except Exception as e:
        print(f"浏览器运行错误: {e}")
    finally:
        if proxy_task and not protocol == "http":
            proxy_task.cancel()

def main(page: ft.Page):
    """主界面"""
    page.title = "Antic Browser - 浏览器指纹保护"
    page.adaptive = True
    page.theme_mode = ft.ThemeMode.DARK

    def config_load(profile: str):
        """加载并运行配置"""
        try:
            with open(f"config/{profile}", "r", encoding="utf-8") as f:
                config = json.load(f)
            
            asyncio.run(run_browser(
                config["user-agent"], 
                config["screen_height"], 
                config["screen_width"], 
                config["timezone"], 
                config["lang"], 
                config["proxy"], 
                config["cookies"], 
                config["webgl"], 
                config["vendor"], 
                config["cpu"], 
                config["ram"], 
                config["is_touch"], 
                profile
            ))
        except Exception as e:
            print(f"加载配置错误: {e}")

    def delete_profile(profile: str):
        """删除配置文件"""
        try:
            os.remove(f"config/{profile}")
            page.controls = get_config_content()
            page.update()
        except Exception as e:
            print(f"删除配置错误: {e}")

    def get_config_content():
        """获取配置文件内容"""
        configs = []
        config_dir = "config"
        
        if not os.path.exists(config_dir):
            os.makedirs(config_dir, exist_ok=True)

        for cfg in os.listdir(config_dir):
            if not cfg.endswith('.json'):
                continue
                
            try:
                with open(f"config/{cfg}", "r", encoding="utf-8") as f:
                    config = json.load(f)

                configs.append(ft.Container(
                    bgcolor=ft.Colors.WHITE24, 
                    padding=20, 
                    border_radius=20, 
                    content=ft.Row([
                        ft.Row([
                            ft.Text(cfg.rsplit(".", 1)[0], size=20, weight=ft.FontWeight.W_600),
                            ft.FilledButton(
                                text=config["lang"], 
                                icon="language", 
                                bgcolor=ft.Colors.WHITE24, 
                                color=ft.Colors.WHITE, 
                                icon_color=ft.Colors.WHITE, 
                                style=ft.ButtonStyle(padding=20)
                            ),
                            ft.FilledButton(
                                text=config["timezone"], 
                                icon="schedule", 
                                bgcolor=ft.Colors.WHITE24, 
                                color=ft.Colors.WHITE, 
                                icon_color=ft.Colors.WHITE, 
                                style=ft.ButtonStyle(padding=20)
                            )
                        ]),
                        ft.Row([
                            ft.IconButton(
                                icon=ft.Icons.DELETE, 
                                icon_color=ft.Colors.WHITE70, 
                                on_click=lambda _, c=cfg: delete_profile(c)
                            ),
                            ft.FilledButton(
                                text="启动", 
                                icon="play_arrow", 
                                style=ft.ButtonStyle(padding=20), 
                                on_click=lambda _, c=cfg: config_load(c)
                            )
                        ])
                    ], alignment=ft.MainAxisAlignment.SPACE_BETWEEN)
                ))
            except Exception as e:
                print(f"读取配置文件错误 {cfg}: {e}")

        if configs:
            config_content = [ft.Column(
                controls=[
                    ft.Text("配置文件", size=20),
                    ft.Column(controls=configs)
                ],
                spacing=20,
                expand=True,
                scroll=ft.ScrollMode.ALWAYS,
                alignment=ft.MainAxisAlignment.CENTER,
                horizontal_alignment=ft.CrossAxisAlignment.CENTER
            )]
        else:
            config_content = [ft.Row(
                [ft.Text("暂无配置文件", size=20)],
                alignment=ft.MainAxisAlignment.CENTER,
            )]

        return config_content

    def get_proxy_list():
        """获取代理列表"""
        proxies = []
        proxy_file = "proxies.txt"
        
        if not os.path.exists(proxy_file):
            with open(proxy_file, "w", encoding="utf-8") as f:
                f.write("")
            return []

        try:
            with open(proxy_file, "r", encoding="utf-8") as f:
                for line in f.read().split("\n"):
                    line = line.strip()
                    if line and not line.startswith("#"):
                        proxies.append(line)
            return proxies
        except Exception:
            return []

    def get_proxies_content():
        """获取代理内容"""
        proxies = []
        proxy_list = get_proxy_list()

        for line in proxy_list:
            try:
                if "@" in line:
                    ip = line.split("://")[1].split("@")[1].split(":")[0]
                else:
                    ip = line.split("://")[1].split(":")[0]

                info = get_proxy_info(ip)

                proxies.append(ft.Container(
                    bgcolor=ft.Colors.WHITE24, 
                    padding=20, 
                    border_radius=20, 
                    content=ft.Row([
                        ft.Text(line, size=16, weight=ft.FontWeight.W_600),
                        ft.FilledButton(
                            text=info["country_code"], 
                            icon="flag", 
                            bgcolor=ft.Colors.WHITE24, 
                            color=ft.Colors.WHITE, 
                            icon_color=ft.Colors.WHITE, 
                            style=ft.ButtonStyle(padding=20)
                        ),
                        ft.FilledButton(
                            text=info["city"], 
                            icon="location_city", 
                            bgcolor=ft.Colors.WHITE24, 
                            color=ft.Colors.WHITE, 
                            icon_color=ft.Colors.WHITE, 
                            style=ft.ButtonStyle(padding=20)
                        ),
                        ft.FilledButton(
                            text=info["timezone"], 
                            icon="schedule", 
                            bgcolor=ft.Colors.WHITE24, 
                            color=ft.Colors.WHITE, 
                            icon_color=ft.Colors.WHITE, 
                            style=ft.ButtonStyle(padding=20)
                        )
                    ])
                ))
            except Exception:
                continue  # 跳过无效代理

        if proxies:
            proxies_content = [ft.Column(
                controls=[
                    ft.Text("代理列表", size=20),
                    ft.Column(controls=proxies)
                ],
                spacing=20,
                expand=True,
                scroll=ft.ScrollMode.ALWAYS,
                alignment=ft.MainAxisAlignment.CENTER,
                horizontal_alignment=ft.CrossAxisAlignment.CENTER 
            )]
        else:
            proxies_content = [ft.Row(
                [ft.Text("暂无代理", size=20)],
                alignment=ft.MainAxisAlignment.CENTER,
            )]

        return proxies_content

    def save_config(e):
        """保存配置文件"""
        try:
            profile_name = profile_name_field.value or "未命名配置"
            user_agent_value = user_agent_field.value or USER_AGENT
            screen_value = screen_dropdown.value or "1920×1080"
            timezone_value = timezone_dropdown.value or "Asia/Shanghai"
            language_value = language_dropdown.value or "zh-CN"
            proxy_value = proxy_dropdown.value or False
            cookies_value = cookies_field.value or False
            webgl_value = webgl_switch.value
            vendor_value = vendor_field.value or "Google Inc."
            cpu_threads_value = int(cpu_threads_field.value) if cpu_threads_field.value else 6
            ram_value = int(ram_field.value) if ram_field.value else 6
            is_touch_value = is_touch_switch.value

            # 确保配置目录存在
            os.makedirs("config", exist_ok=True)
            
            config_data = {
                "user-agent": user_agent_value,
                "screen_height": int(screen_value.split("×")[1]),
                "screen_width": int(screen_value.split("×")[0]),
                "timezone": timezone_value,
                "lang": language_value,
                "proxy": proxy_value,
                "cookies": cookies_value,
                "webgl": webgl_value,
                "vendor": vendor_value,
                "cpu": cpu_threads_value,
                "ram": ram_value,
                "is_touch": is_touch_value
            }
            
            with open(f"config/{profile_name}.json", "w", encoding="utf-8") as f:
                json.dump(obj=config_data, fp=f, indent=4, ensure_ascii=False)

            page.controls = get_config_content()
            page.update()
            
            # 显示成功消息
            page.snack_bar = ft.SnackBar(ft.Text("配置保存成功!"))
            page.snack_bar.open = True
            page.update()
            
        except Exception as e:
            page.snack_bar = ft.SnackBar(ft.Text(f"保存配置错误: {e}"))
            page.snack_bar.open = True
            page.update()

    def open_config_page(e):
        """打开配置页面"""
        nonlocal profile_name_field, user_agent_field, screen_dropdown, timezone_dropdown
        nonlocal language_dropdown, proxy_dropdown, cookies_field, webgl_switch
        nonlocal vendor_field, cpu_threads_field, ram_field, is_touch_switch

        n = 1
        while os.path.isfile(f"config/配置_{n}.json"):
            n += 1

        profile_name_field = ft.TextField(
            label="配置名称", 
            value=f"配置_{n}", 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10
        )
        
        user_agent_field = ft.TextField(
            hint_text="用户代理(User Agent)", 
            value=USER_AGENT, 
            expand=True, 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10,
            multiline=True
        )
        
        screen_dropdown = ft.Dropdown(
            label="屏幕分辨率",
            value="1920×1080",
            width=300,
            border_color=ft.Colors.WHITE,
            border_radius=20,
            options=[ft.dropdown.Option(screen) for screen in SCREENS]
        )
        
        timezone_dropdown = ft.Dropdown(
            label="时区",
            value="Asia/Shanghai",
            width=350,
            border_color=ft.Colors.WHITE,
            border_radius=20,
            options=[ft.dropdown.Option(timezone) for timezone in TIMEZONES]
        )
        
        language_dropdown = ft.Dropdown(
            label="语言",
            value="zh-CN",
            width=200,
            border_color=ft.Colors.WHITE,
            border_radius=20,
            options=[ft.dropdown.Option(lang) for lang in LANGUAGES]
        )
        
        proxy_dropdown = ft.Dropdown(
            label="代理",
            hint_text="选择代理服务器",
            expand=True,
            border_color=ft.Colors.WHITE,
            border_radius=20,
            options=[ft.dropdown.Option(proxy) for proxy in get_proxy_list()]
        )
        
        cookies_field = ft.TextField(
            hint_text="Cookies文件路径", 
            expand=True, 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10
        )
        
        webgl_switch = ft.Switch(
            adaptive=True,
            label="启用WebGL",
            value=True,
        )
        
        vendor_field = ft.TextField(
            label="浏览器厂商", 
            value="Google Inc.", 
            expand=True, 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10
        )
        
        cpu_threads_field = ft.TextField(
            label="CPU核心数", 
            value="6", 
            keyboard_type=ft.KeyboardType.NUMBER, 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10
        )
        
        ram_field = ft.TextField(
            label="内存大小(GB)", 
            value="6", 
            keyboard_type=ft.KeyboardType.NUMBER, 
            border_color=ft.Colors.WHITE, 
            border_radius=20, 
            content_padding=10
        )
        
        is_touch_switch = ft.Switch(
            adaptive=True,
            label="触摸支持",
            value=False,
        )

        page.controls = [ft.Column(
            controls=[
                ft.Text("新建浏览器配置", size=24, weight=ft.FontWeight.BOLD),
                ft.Container(
                    padding=20,
                    content=ft.Row(
                        [
                            profile_name_field,
                            ft.FilledButton(
                                text="保存配置", 
                                icon="check", 
                                style=ft.ButtonStyle(padding=20), 
                                on_click=save_config
                            )
                        ],
                        alignment=ft.MainAxisAlignment.SPACE_BETWEEN
                    )
                ),
                ft.Container(
                    padding=20,
                    content=ft.Row(
                        [
                            user_agent_field,
                            screen_dropdown
                        ]
                    )
                ),
                ft.Container(
                    padding=20,
                    content=ft.Row(
                        spacing=10,
                        controls=[
                            timezone_dropdown,
                            language_dropdown,
                            proxy_dropdown
                        ]
                    ),
                ),
                ft.Container(
                    padding=20,
                    content=ft.Row(
                        [
                            cookies_field,
                            webgl_switch
                        ]
                    )
                ),
                ft.Container(
                    padding=20,
                    content=ft.Row(
                        [
                            vendor_field,
                            cpu_threads_field,
                            ram_field,
                            is_touch_switch
                        ]
                    )
                ),
                ft.Container(
                    padding=20,
                    content=ft.Text("提示: 配置保存后可以在主页面启动浏览器实例", size=14, color=ft.Colors.GREY)
                )
            ],
            spacing=5,
            alignment=ft.MainAxisAlignment.CENTER,
            horizontal_alignment=ft.CrossAxisAlignment.CENTER 
        )]

        page.update()

    def update_content(e):
        """更新页面内容"""
        if e.control.selected_index == 0:
            page.appbar = ft.AppBar(
                title=ft.Text("Antic Browser - 配置文件"),
                actions=[
                    ft.IconButton(
                        ft.Icons.ADD, 
                        tooltip="新建配置",
                        style=ft.ButtonStyle(padding=10), 
                        on_click=open_config_page
                    )
                ],
                bgcolor=ft.Colors.with_opacity(0.04, ft.CupertinoColors.SYSTEM_BACKGROUND),
            )
            page.controls = get_config_content()
        elif e.control.selected_index == 1:
            page.appbar = ft.AppBar(
                title=ft.Text("Antic Browser - 代理管理"),
                actions=[],
                bgcolor=ft.Colors.with_opacity(0.04, ft.CupertinoColors.SYSTEM_BACKGROUND),
            )
            page.controls = get_proxies_content()

        page.update()

    # 初始化页面
    page.appbar = ft.AppBar(
        title=ft.Text("Antic Browser - 浏览器指纹保护"),
        actions=[
            ft.IconButton(
                ft.Icons.ADD, 
                tooltip="新建配置",
                style=ft.ButtonStyle(padding=10), 
                on_click=open_config_page
            )
        ],
        bgcolor=ft.Colors.with_opacity(0.04, ft.CupertinoColors.SYSTEM_BACKGROUND),
    )

    page.navigation_bar = ft.NavigationBar(
        on_change=update_content,
        destinations=[
            ft.NavigationBarDestination(icon=ft.Icons.SETTINGS, label="配置文件"),
            ft.NavigationBarDestination(icon=ft.Icons.SECURITY, label="代理管理")
        ],
        border=ft.Border(
            top=ft.BorderSide(color=ft.CupertinoColors.SYSTEM_GREY2, width=0)
        ),
    )

    page.add(get_config_content()[0])

if __name__ == "__main__":
    # 初始化必要的目录和文件
    os.makedirs("config", exist_ok=True)
    os.makedirs("cookies", exist_ok=True)

    # 下载GeoIP数据库（如果不存在）
    def download_file(url, path):
        if not os.path.isfile(path):
            try:
                print(f"正在下载 {path}...")
                response = requests.get(url, timeout=30)
                with open(path, "wb") as file:
                    file.write(response.content)
                print(f"下载完成: {path}")
            except Exception as e:
                print(f"下载 {path} 失败: {e}")

    # 尝试下载GeoIP数据库
    try:
        download_file("https://git.io/GeoLite2-Country.mmdb", COUNTRY_DATABASE_PATH)
        download_file("https://git.io/GeoLite2-City.mmdb", CITY_DATABASE_PATH)
    except Exception as e:
        print(f"初始化GeoIP数据库失败: {e}")

    # 启动应用
    ft.app(main)
