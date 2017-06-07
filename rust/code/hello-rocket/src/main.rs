#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[get("/world")]
fn world() -> &'static str {
    "Hello, world!"
}

#[get("/hello/<name>")]
fn hello(name: &str) -> String {
    format!("Hello, {}!", name)
}

fn main() {
    rocket::ignite()
		.mount("/hello", routes![world])
		.mount("/", routes![hello])
		.launch();
}