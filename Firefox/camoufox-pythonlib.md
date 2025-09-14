
- https://github.com/daijro/camoufox/tree/main/pythonlib

## 
- camoufox\pythonlib\camoufox\pkgman.py
```python
LAUNCH_FILE = {
    'win': 'camoufox.exe',
    'mac': '../MacOS/camoufox',
    'lin': 'camoufox-bin',
}

def launch_path() -> str:
    """
    Get the path to the camoufox executable.
    """
    launch_path = get_path(LAUNCH_FILE[OS_NAME])
    if not os.path.exists(launch_path):
        # Not installed error
        raise CamoufoxNotInstalled(
            f"Camoufox is not installed at {camoufox_path()}. Please run `camoufox fetch` to install."
        )
    return launch_path
```

- camoufox\pythonlib\camoufox\utils.py
```python
def launch_options(

    *,
    config: Optional[Dict[str, Any]] = None,
    os: Optional[ListOrString] = None,
    block_images: Optional[bool] = None,
    block_webrtc: Optional[bool] = None,
    block_webgl: Optional[bool] = None,
    disable_coop: Optional[bool] = None,
    webgl_config: Optional[Tuple[str, str]] = None,
    geoip: Optional[Union[str, bool]] = None,
    humanize: Optional[Union[bool, float]] = None,
    locale: Optional[Union[str, List[str]]] = None,
    addons: Optional[List[str]] = None,
    fonts: Optional[List[str]] = None,
    custom_fonts_only: Optional[bool] = None,
    exclude_addons: Optional[List[DefaultAddons]] = None,
    screen: Optional[Screen] = None,
    window: Optional[Tuple[int, int]] = None,
    fingerprint: Optional[Fingerprint] = None,
    ff_version: Optional[int] = None,
    headless: Optional[bool] = None,
    main_world_eval: Optional[bool] = None,
    executable_path: Optional[Union[str, Path]] = None,
    firefox_user_prefs: Optional[Dict[str, Any]] = None,
    proxy: Optional[Dict[str, str]] = None,
    enable_cache: Optional[bool] = None,
    args: Optional[List[str]] = None,
    env: Optional[Dict[str, Union[str, float, bool]]] = None,
    i_know_what_im_doing: Optional[bool] = None,
    debug: Optional[bool] = None,
    virtual_display: Optional[str] = None,
    **launch_options: Dict[str, Any],
) -> Dict[str, Any]:
    # Build the config
    if config is None:
        config = {}

    # Set default values for optional arguments
    if headless is None:
        headless = False
    if addons is None:
        addons = []
    if args is None:
        args = []
    if firefox_user_prefs is None:
        firefox_user_prefs = {}
    if custom_fonts_only is None:
        custom_fonts_only = False
    if i_know_what_im_doing is None:
        i_know_what_im_doing = False
    if env is None:
        env = cast(Dict[str, Union[str, float, bool]], environ)
    if isinstance(executable_path, str):
        # Convert executable path to a Path object
        executable_path = Path(abspath(executable_path))

    # Handle virtual display
    if virtual_display:
        env['DISPLAY'] = virtual_display

    # Warn the user for manual config settings
    if not i_know_what_im_doing:
        warn_manual_config(config)

    # Assert the target OS is valid
    if os:
        check_valid_os(os)

    # webgl_config requires OS to be set
    elif webgl_config:
        raise ValueError('OS must be set when using webgl_config')

    # Add the default addons
    add_default_addons(addons, exclude_addons)

    # Confirm all addon paths are valid
    if addons:
        confirm_paths(addons)
        config['addons'] = addons

    # Get the Firefox version
    if ff_version:
        ff_version_str = str(ff_version)
        LeakWarning.warn('ff_version', i_know_what_im_doing)
    else:
        ff_version_str = installed_verstr().split('.', 1)[0]

    # Generate a fingerprint
    if fingerprint is None:
        fingerprint = generate_fingerprint(
            screen=screen or get_screen_cons(headless or 'DISPLAY' in env),
            window=window,
            os=os,
        )
    else:
        # Or use the one passed by the user
        if not i_know_what_im_doing:
            check_custom_fingerprint(fingerprint)

    # Inject the fingerprint into the config
    merge_into(
        config,
        from_browserforge(fingerprint, ff_version_str),
    )

    target_os = get_target_os(config)

    # Set a random window.history.length
    set_into(config, 'window.history.length', randrange(1, 6))  # nosec

    # Update fonts list
    if fonts:
        config['fonts'] = fonts

    if custom_fonts_only:
        firefox_user_prefs['gfx.bundled-fonts.activate'] = 0
        if fonts:
            # The user has passed their own fonts, and OS fonts are disabled.
            LeakWarning.warn('custom_fonts_only')
        else:
            # OS fonts are disabled, and the user has not passed their own fonts either.
            raise ValueError('No custom fonts were passed, but `custom_fonts_only` is enabled.')
    else:
        update_fonts(config, target_os)

    # Set a fixed font spacing seed
    set_into(config, 'fonts:spacing_seed', randint(0, 1_073_741_823))  # nosec

    # Set geolocation
    if geoip:
        geoip_allowed()  # Assert that geoip is allowed

        if geoip is True:
            # Find the user's IP address
            if proxy:
                geoip = public_ip(Proxy(**proxy).as_string())
            else:
                geoip = public_ip()

        # Spoof WebRTC if not blocked
        if not block_webrtc:
            if valid_ipv4(geoip):
                set_into(config, 'webrtc:ipv4', geoip)
                firefox_user_prefs['network.dns.disableIPv6'] = True
            elif valid_ipv6(geoip):
                set_into(config, 'webrtc:ipv6', geoip)

        geolocation = get_geolocation(geoip)
        config.update(geolocation.as_config())

    # Raise a warning when a proxy is being used without spoofing geolocation.
    # This is a very bad idea; the warning cannot be ignored with i_know_what_im_doing.
    elif (
        proxy
        and 'localhost' not in proxy.get('server', '')
        and not is_domain_set(config, 'geolocation')
    ):
        LeakWarning.warn('proxy_without_geoip')

    # Set locale
    if locale:
        handle_locales(locale, config)

    # Pass the humanize option
    if humanize:
        set_into(config, 'humanize', True)
        if isinstance(humanize, (int, float)):
            set_into(config, 'humanize:maxTime', humanize)

    # Enable the main world context creation
    if main_world_eval:
        set_into(config, 'allowMainWorld', True)

    # Set Firefox user preferences
    if block_images:
        LeakWarning.warn('block_images', i_know_what_im_doing)
        firefox_user_prefs['permissions.default.image'] = 2
    if block_webrtc:
        firefox_user_prefs['media.peerconnection.enabled'] = False
    if disable_coop:
        LeakWarning.warn('disable_coop', i_know_what_im_doing)
        firefox_user_prefs['browser.tabs.remote.useCrossOriginOpenerPolicy'] = False

    # Allow allow_webgl parameter for backwards compatibility
    if block_webgl or launch_options.pop('allow_webgl', True) is False:
        firefox_user_prefs['webgl.disabled'] = True
        LeakWarning.warn('block_webgl', i_know_what_im_doing)
    else:
        # If the user has provided a specific WebGL vendor/renderer pair, use it
        if webgl_config:
            webgl_fp = sample_webgl(target_os, *webgl_config)
        else:
            webgl_fp = sample_webgl(target_os)
        enable_webgl2 = webgl_fp.pop('webGl2Enabled')

        # Merge the WebGL fingerprint into the config
        merge_into(config, webgl_fp)
        # Set the WebGL preferences
        merge_into(
            firefox_user_prefs,
            {
                'webgl.enable-webgl2': enable_webgl2,
                'webgl.force-enabled': True,
            },
        )

    # Canvas anti-fingerprinting
    merge_into(
        config,
        {
            'canvas:aaOffset': randint(-50, 50),  # nosec
            'canvas:aaCapOffset': True,
        },
    )

    # Cache previous pages, requests, etc (uses more memory)
    if enable_cache:
        merge_into(firefox_user_prefs, CACHE_PREFS)

    # Print the config if debug is enabled
    if debug:
        print('[DEBUG] Config:')
        pprint(config)

    # Validate the config
    validate_config(config, path=executable_path)

    # Prepare environment variables to pass to Camoufox
    env_vars = {
        **get_env_vars(config, target_os),
        **env,
    }
    # Prepare the executable path
    if executable_path:
        executable_path = str(executable_path)
    else:
        executable_path = launch_path()

    return {
        "executable_path": executable_path,
        "args": args,
        "env": env_vars,
        "firefox_user_prefs": firefox_user_prefs,
        "proxy": proxy,
        "headless": headless,
        **(launch_options if launch_options is not None else {}),
    }
```
