```go
package main

import (
    "fmt"
    "github.com/dgrijalva/jwt-go"
)
 
func main() {
    tokenString := "xxxxx" // 要解析的JWT令牌字符串
    
    // 创建一个新的Token对象
    token, err := jwt.Parse(tokenString, func(token *jwt.Token) (interface{}, error) {
        return []byte("key"), nil // JWT密钥
    })
    
    if claims, ok := token.Claims.(jwt.MapClaims); ok && token.Valid {
        fmt.Printf("解析成功！%#v", claims)
        
    } else if ve, ok := err.(*jwt.ValidationError); ok {
        if ve.Errors&jwt.ValidationErrorMalformed != 0 {
            fmt.Println("无效的令牌格式。")
        } else if ve.Errors&(jwt.ValidationErrorExpired|jwt.ValidationErrorNotValidYet) != 0 {
            fmt.Println("过期或未生效的令牌。")
        } else {
            fmt.Println("其他错误。")
        }
    } else {
        fmt.Println("解析失败。")
    }
}
```
