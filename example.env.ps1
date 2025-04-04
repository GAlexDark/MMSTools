# The $PROXY_NAME variable value must be contains DND name only (without http:// and the slashes)
[string] $PROXY_NAME = ""
[string] $PROXY_PORT = ""

[string] $REMOTE_HOST = "192.0.2.2"
[int] $REMOTE_PORT = 22
# The $REMOTE_PATH variable value must end with a '/' !!!!
[string] $REMOTE_PATH = "/home/mr_data/upload/"

# The $LOCAL_STORAGE variable value must end with a '*' !!!!
[string] $LOCAL_STORAGE = "B:\datasource\incoming\*"
[string] $ARCHIVE_STORAGE = "B:\datasource\archive"

[string] $DATETIME_FORMAT = "dd.MM.yyyy HH:mm:ss"
[string[]] $FILE_TYPES = ('*.p7s','*.xlsx')

[string] $DB_NAME = "example.db"
