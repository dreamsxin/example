#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

use std::io;
use std::path::{Path, PathBuf};

use rocket::response::NamedFile;

#[get("/world")]
fn world() -> &'static str {
    "Hello, world!"
}

#[get("/hello/<name>")]
fn hello(name: &str) -> String {
    format!("Hello, {}!", name)
}

#[get("/hello/<name>/<age>/<cool>")]
fn hello2(name: &str, age: u8, cool: bool) -> String {
    if cool {
      format!("You're a cool {} year old, {}!", age, name)
    } else {
      format!("{}, we need to talk about your coolness.", name)
    }
}

#[get("/page/<file..>")]
fn files(file: PathBuf) -> Option<NamedFile> {
    NamedFile::open(Path::new("static/").join(file)).ok()
}

fn main() {
    rocket::ignite()
		.mount("/hello", routes![world])
		.mount("/", routes![hello, hello2, files])
		.launch();
}