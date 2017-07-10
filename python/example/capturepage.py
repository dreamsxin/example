# sudo apt-get install python-selenium
from selenium import webdriver
import time

def capture(url, save_fn="capture.png"):
  browser = webdriver.Firefox() # Get local session of firefox
  browser.set_window_size(1200, 900)
  browser.get(url) # Load page
  browser.execute_script("""
    (function () {
      var y = 0;
      var step = 100;
      window.scroll(0, 0);
 
      function f() {
        if (y < document.body.scrollHeight) {
          y += step;
          window.scroll(0, y);
          setTimeout(f, 50);
        } else {
          window.scroll(0, 0);
          document.title += "scroll-done";
        }
      }
 
      setTimeout(f, 1000);
    })();
  """)
 
  for i in xrange(30):
    if "scroll-done" in browser.title:
      break
    time.sleep(1)
 
  browser.save_screenshot(save_fn)
  browser.close()
 
 
if __name__ == "__main__":
 
  capture("http://myleftstudio.com")