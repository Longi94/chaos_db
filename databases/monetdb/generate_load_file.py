import os

with open('load.sql', 'r') as f:
    data = f.read()
    data = data.replace('{DIR}', os.getcwd())

with open('load.sql.tmp', 'w') as f:
    f.write(data)
