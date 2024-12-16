
## wasm-bindgen

`Cargo.toml`
```toml
[lib]
# if you want to integrate your rust code with javascript we use cdylib
crate-type=["cdylib"]

[dependencies]
wasm-bindgen="0.2.63"
wee_alloc="0.4.5"
```

`index.js`:
```js
import * as wasm from "testing-wasm";

export const jsfunc = () => {
  console.log("jsfunc called");
};

// Call Rust from JS. This function will call `jsfunc`, declared above.
wasm.rustfunc();
```

`lib.rs`:
```rs
mod utils;

use wasm_bindgen::prelude::*;

// When the `wee_alloc` feature is enabled, use `wee_alloc` as the global
// allocator.
#[cfg(feature = "wee_alloc")]
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

#[wasm_bindgen(module = "/www/index.js")]
extern "C" {
    fn jsfunc();
}

#[wasm_bindgen]
pub fn rustfunc() {
    // call JS
    jsfunc();
}
```
