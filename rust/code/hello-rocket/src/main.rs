#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[get("/world")]
fn world() -> &'static str {
    "Hello, world!"
}

fn main() {
    rocket::ignite()
		.mount("/hello", routes![world])
		.launch();
}