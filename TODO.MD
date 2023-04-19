# Todo / Error List

## ConfigServer
### Error
- [ ] Segmentation fault whit location without index
  ```c++
  location /upload {
    root data/cgi;
  }
   ```
- [ ] Config file don't accept folder with other char than a-z like `cgi-bin`

- [ ] ServerNames not works? for CGI