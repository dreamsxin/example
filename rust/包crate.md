#

Rust 为我们提供了强大的包管理工具：

- 项目(Package)：可以用来构建、测试和分享包
- 工作空间(WorkSpace)：对于大型项目，可以进一步将多个包联合在一起，组织成工作空间
- 包(Crate)：一个由多个模块组成的树形结构，可以作为三方库进行分发，也可以生成可执行文件进行运行
- 模块(Module)：可以一个文件多个模块，也可以一个文件一个模块，模块可以被认为是真实项目中的代码组织单元

## 项目 Package
鉴于 Rust 团队标新立异的起名传统，以及包的名称被 crate 占用，库的名称被 library 占用，经过斟酌， 我们决定将 Package 翻译成项目，你也可以理解为工程、软件包。

由于 Package 就是一个项目，因此它包含有独立的 Cargo.toml 文件，以及因为功能性被组织在一起的一个或多个包。一个 Package 只能包含一个库(library)类型的包，但是可以包含多个二进制可执行类型的包。


创建二进制 Package：
```shell
cargo new my-project
```
创建库 Package：
```shell
cargo new my-lib --lib
```

## 模块
`lib.rs`：
```rust
// 餐厅前厅，用于吃饭
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist() {}

        fn seat_at_table() {}
    }

    mod serving {
        fn take_order() {}

        fn serve_order() {}

        fn take_payment() {}
    }
}

mod front_of_house {
    mod hosting {
        fn add_to_waitlist() {}
    }
}
```
**模块与文件分离**
将 `front_of_house` 单独保存到 `front_of_house.rs` 文件（或者保存在`front_of_house/mod.rs`目录名为模块名）：
```rust
pub mod hosting {
    pub fn add_to_waitlist() {}

    fn seat_at_table() {}
}
```
`lib.rs`：
```rust
mod front_of_house; // 告诉 Rust 从另一个和模块 front_of_house 同名的文件中加载该模块的内容

pub use crate::front_of_house::hosting;

pub fn eat_at_restaurant() {
    hosting::add_to_waitlist();
    hosting::add_to_waitlist();
    hosting::add_to_waitlist();
}
```

### 使用 crate 引用模块
包根
```
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist() {}
    }
}

pub fn eat_at_restaurant() {
    // 绝对路径
    crate::front_of_house::hosting::add_to_waitlist();

    // 相对路径
    front_of_house::hosting::add_to_waitlist();
}
```

### super 引用模块
super 代表的是父模块为开始的引用方式
```rust
fn serve_order() {}

// 厨房模块
mod back_of_house {
    fn fix_incorrect_order() {
        cook_order();
        super::serve_order();
    }

    fn cook_order() {}
}
```
## 使用 self 引用模块
self 其实就是引用自身模块中的项
```rust
fn serve_order() {
    self::back_of_house::cook_order()
}

mod back_of_house {
    fn fix_incorrect_order() {
        cook_order();
        crate::serve_order();
    }

    pub fn cook_order() {}
}
```
