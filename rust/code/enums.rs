#[derive(Debug)]
enum IpAddr {
    V4(u8, u8, u8, u8),
    V6(String),
}

#[derive(Debug)]
enum UsState {
    Alabama,
    Alaska,
}

enum Coin {
    Penny,
    Nickel,
	Dime,
    Quarter(UsState),
}

fn value_in_cents(coin: Coin) -> i32 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("State quarter from {:?}!", state);
            25
        },
    }
}

fn main() {
	let home = IpAddr::V4(127, 0, 0, 1);
	let loopback = IpAddr::V6(String::from("::1"));

    println!("home is {:?}", home);
    println!("loopback is {:?}", loopback);

    println!("Penny is {:?}", value_in_cents(Coin::Penny)); // Prints "1".
    println!("Nickel is {:?}", value_in_cents(Coin::Nickel)); // Prints "5".
    println!("Quarter is {:?}", value_in_cents(Coin::Quarter(UsState::Alaska))); // Prints "25".
}