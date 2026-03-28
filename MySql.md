# SvenEnhancer MySql Support
To use MySQL features, the libmysql.dll file (or libmysql.so for Linux) must be present in the addons/metamod/dlls/se_libs directory.
If this file is present, MySQL features will become available.

Because the 32-bit version does not support the legacy authentication method, accounts using caching_sha2_password cannot be logged into. To be able to log in, you must use the mysql_native_password authentication method.
For example for mysql_native_password
```SQL
CREATE USER 'sven'@'localhost'
IDENTIFIED WITH mysql_native_password BY '1234';
FLUSH PRIVILEGES;
```

For Update

```SQL
ALTER USER 'sven'@'localhost'
IDENTIFIED WITH mysql_native_password BY '1234';
FLUSH PRIVILEGES;
```

After this process, the server will be in a state where it can be connected to.


# MySql For Angelscript
[Sample Plugin](https://github.com/ltibm/SvenEnhancer/blob/master/SamplePlugin/test_mysql.as)
