# crow

https://github.com/ipkn/crow

## 简单例子 Hello world

```cpp
#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([]() {
        return "Hello world!";
    });

    app.port(18080).run();
}
```

- 路由 route

```cpp
#ifdef CROW_MSVC_WORKAROUND
# define CROW_ROUTE(app, url) app.route_dynamic(url)
#else
# define CROW_ROUTE(app, url) app.route<crow::black_magic::get_parameter_tag(url)>(url)
#endif
```

## 请求参数

```cpp
#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/params")
    ([](const crow::request& req){
        std::ostringstream os;
        os << "Params: " << req.url_params << "\n\n"; 
        os << "The key 'foo' was " << (req.url_params.get("foo") == nullptr ? "not " : "") << "found.\n";
        if(req.url_params.get("pew") != nullptr) {
            double countD = boost::lexical_cast<double>(req.url_params.get("pew"));
            os << "The value of 'pew' is " <<  countD << '\n';
        }
        auto count = req.url_params.get_list("count");
        os << "The key 'count' contains " << count.size() << " value(s).\n";
        for(const auto& countVal : count) {
            os << " - " << countVal << '\n';
        }
        return crow::response{os.str()};
    });

    app.port(18080)
        .multithreaded()
        .run();
}
```

## 使用 JSON 数据格式

```cpp
#include "crow.h"

int main()
{
    crow::SimpleApp app;

    // simple json response
    CROW_ROUTE(app, "/json")
    ([]{
        crow::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
    });

    // more json example
    CROW_ROUTE(app, "/add_json")
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        int sum = x["a"].i()+x["b"].i();
        std::ostringstream os;
        os << sum;
        return crow::response{os.str()};
    });

    app.port(18080)
        .multithreaded()
        .run();
}
```

## 请求头信息

```cpp
#include "crow.h"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
    ([](const crow::request& req){
        std::ostringstream os;
        // ci_map req.header
        os << "Headers Server: " << req.get_header_value("test") << "\n\n"; 
        return req.get_header_value("test");
    });

    app.port(18080)
        .multithreaded()
        .run();
}
```

## 中间件

```cpp
#include "crow.h"

#include <sstream>

class ExampleLogHandler : public crow::ILogHandler {
    public:
        void log(std::string message, crow::LogLevel level) override {
//            cerr << "ExampleLogHandler -> " << message;
        }
};

struct ExampleMiddleware 
{
    std::string message;

    ExampleMiddleware() 
    {
        message = "foo";
    }

    void setMessage(std::string newMsg)
    {
        message = newMsg;
    }

    struct context
    {
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        CROW_LOG_DEBUG << " - MESSAGE: " << message;
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {
        // no-op
    }
};

int main()
{
    crow::App<ExampleMiddleware> app;

    app.get_middleware<ExampleMiddleware>().setMessage("hello");

    app.route_dynamic("/")
    ([]{
        return "Hello World!";
    });

    app.route_dynamic("/about")
    ([](){
        return "About Crow example.";
    });

    // a request to /path should be forwarded to /path/
    app.route_dynamic("/path/")
    ([](){
        return "Trailing slash test case..";
    });

    // simple json response
    app.route_dynamic("/json")
    ([]{
        crow::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
    });

    app.route_dynamic("/hello/<int>")
    ([](int count){
        if (count > 100)
            return crow::response(400);
        std::ostringstream os;
        os << count << " bottles of beer!";
        return crow::response(os.str());
    });

    app.route_dynamic("/add/<int>/<int>")
    ([](const crow::request& req, crow::response& res, int a, int b){
        std::ostringstream os;
        os << a+b;
        res.write(os.str());
        res.end();
    });

    // Compile error with message "Handler type is mismatched with URL paramters"
    //CROW_ROUTE(app,"/another/<int>")
    //([](int a, int b){
        //return crow::response(500);
    //});

    // more json example
    app.route_dynamic("/add_json")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        auto sum = x["a"].i()+x["b"].i();
        std::ostringstream os;
        os << sum;
        return crow::response{os.str()};
    });

    app.route_dynamic("/params")
    ([](const crow::request& req){
        std::ostringstream os;
        os << "Params: " << req.url_params << "\n\n"; 
        os << "The key 'foo' was " << (req.url_params.get("foo") == nullptr ? "not " : "") << "found.\n";
        if(req.url_params.get("pew") != nullptr) {
            double countD = boost::lexical_cast<double>(req.url_params.get("pew"));
            os << "The value of 'pew' is " <<  countD << '\n';
        }
        auto count = req.url_params.get_list("count");
        os << "The key 'count' contains " << count.size() << " value(s).\n";
        for(const auto& countVal : count) {
            os << " - " << countVal << '\n';
        }
        return crow::response{os.str()};
    });    

    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::DEBUG);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
        .multithreaded()
        .run();
}
```

## Websocket

```cpp
#include "crow.h"
#include <unordered_set>
#include <mutex>


int main()
{
    crow::SimpleApp app;

    std::mutex mtx;;
    std::unordered_set<crow::websocket::connection*> users;

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& conn){
                CROW_LOG_INFO << "new websocket connection";
                std::lock_guard<std::mutex> _(mtx);
                users.insert(&conn);
                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                std::lock_guard<std::mutex> _(mtx);
                users.erase(&conn);
                })
        .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary){
                std::lock_guard<std::mutex> _(mtx);
                for(auto u:users)
                    if (is_binary)
                        u->send_binary(data);
                    else
                        u->send_text(data);
                });

    CROW_ROUTE(app, "/")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
	
        auto page = crow::mustache::load("ws.html");
        return page.render(x);
     });

    app.port(40080)
        .multithreaded()
        .run();
}
```
对应的 `templates/ws.html`：
```html
<!doctype html>
<html>
<head>
    <script src="https://code.jquery.com/jquery-3.1.0.min.js"></script>
</head>
<body>
    <input id="msg" type="text"></input>
    <button id="send">
        Send
    </button><BR>
    <textarea id="log" cols=100 rows=50>
    </textarea>
    <script>
var sock = new WebSocket("ws://{{servername}}:40080/ws");

sock.onopen = ()=>{
    console.log('open')
}
sock.onerror = (e)=>{
    console.log('error',e)
}
sock.onclose = ()=>{
    console.log('close')
}
sock.onmessage = (e)=>{
    $("#log").val(
            e.data +"\n" + $("#log").val());
}
$("#msg").keypress(function(e){
    if (e.which == 13)
    {
    sock.send($("#msg").val());
    $("#msg").val("");
    }
});
$("#send").click(()=>{
    sock.send($("#msg").val());
    $("#msg").val("");
});
    </script>
</body>
</html>
```

* chat

```cpp
#include "crow.h"
#include <string>
#include <vector>
#include <chrono>

using namespace std;

vector<string> msgs;
vector<pair<crow::response*, decltype(chrono::steady_clock::now())>> ress;

void broadcast(const string& msg)
{
    msgs.push_back(msg);
    crow::json::wvalue x;
    x["msgs"][0] = msgs.back();
    x["last"] = msgs.size();
    string body = crow::json::dump(x);
    for(auto p : ress)
    {
        auto* res = p.first;
        CROW_LOG_DEBUG << res << " replied: " << body;
        res->end(body);
    }
    ress.clear();
}
// To see how it works go on {ip}:40080 but I just got it working with external build (not directly in IDE, I guess a problem with dependency)
int main()
{
    crow::SimpleApp app;
    crow::mustache::set_base(".");

    CROW_ROUTE(app, "/")
    ([]{
        crow::mustache::context ctx;
        return crow::mustache::load("example_chat.html").render();
    });

    CROW_ROUTE(app, "/logs")
    ([]{
        CROW_LOG_INFO << "logs requested";
        crow::json::wvalue x;
        int start = max(0, (int)msgs.size()-100);
        for(int i = start; i < (int)msgs.size(); i++)
            x["msgs"][i-start] = msgs[i];
        x["last"] = msgs.size();
        CROW_LOG_INFO << "logs completed";
        return x;
    });

    CROW_ROUTE(app, "/logs/<int>")
    ([](const crow::request& /*req*/, crow::response& res, int after){
        CROW_LOG_INFO << "logs with last " << after;
        if (after < (int)msgs.size())
        {
            crow::json::wvalue x;
            for(int i = after; i < (int)msgs.size(); i ++)
                x["msgs"][i-after] = msgs[i];
            x["last"] = msgs.size();

            res.write(crow::json::dump(x));
            res.end();
        }
        else
        {
            vector<pair<crow::response*, decltype(chrono::steady_clock::now())>> filtered;
            for(auto p : ress)
            {
                if (p.first->is_alive() && chrono::steady_clock::now() - p.second < chrono::seconds(30))
                    filtered.push_back(p);
                else
                    p.first->end();
            }
            ress.swap(filtered);
            ress.push_back({&res, chrono::steady_clock::now()});
            CROW_LOG_DEBUG << &res << " stored " << ress.size();
        }
    });

    CROW_ROUTE(app, "/send")
        .methods("GET"_method, "POST"_method)
    ([](const crow::request& req)
    {
        CROW_LOG_INFO << "msg from client: " << req.body;
        broadcast(req.body);
        return "";
    });

    app.port(40080)
        //.multithreaded()
        .run();
}
```

对应的 `html`：
```html
<html>
<head>
<script src="//code.jquery.com/jquery-1.11.0.min.js"></script>
</head>
<body>
<input id="msg" type="text">
<button id="send">Send</button>
<div id="logs">
</div>
<script>
$(document).ready(function(){
	$("#send").click(function(){
		var msg = $("#msg").val();
		console.log(msg);
		if (msg.length > 0)
			$.post("/send", msg);
		$("#msg").val("");
	});
	$("#msg").keyup(function(event){
		if(event.keyCode == 13){
			$("#send").click();
		}
	});
	var lastLog = 0;
	var updateLog;
	updateLog = function(data)
	{
		console.log("recv ");
		console.log(data);
		var lastLog = data.last*1;
		console.log("lastLog: " + lastLog);
		var s = "";
        function htmlEncode(s)
        {
            return s.replace(/&(?!\w+([;\s]|$))/g, "&amp;")
            .replace(/</g, "&lt;").replace(/>/g, "&gt;");
        }
		for(var x in data.msgs)
		{

			s = htmlEncode(data.msgs[x]) + "<BR>" + s;
		}
		$("#logs").html(s+$("#logs").html());
		var failFunction;
		failFunction = function(){
			$.getJSON("/logs/"+lastLog, updateLog).fail(failFunction);
		};
		$.getJSON("/logs/"+lastLog, updateLog).fail(failFunction);
	}
	$.getJSON("/logs", updateLog);
});
</script>
</body>
</html>
```