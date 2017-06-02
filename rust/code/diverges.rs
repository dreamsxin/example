fn main() {
    diverges();
}

fn diverges() -> ! {
    panic!("This function never returns!");
}
