#

Enigo 是一个用 Rust 编写的跨平台输入模拟库。它允许开发者模拟鼠标和键盘输入事件，就像是由实际硬件产生的一样。Enigo 支持 Linux（X11）、macOS 和 Windows 操作系统。

https://github.com/enigo-rs/enigo

```rust
use enigo::{Enigo, KeyboardControllable, Key};
 
fn main() {
    let mut enigo = Enigo::new();
 
    enigo.key_down(Key::Control);
    enigo.key_click(Key::Layout('c'));
    enigo.key_up(Key::Control);
}
```
