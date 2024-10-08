import sqlite3

# 連接到 SQLite 數據庫
conn = sqlite3.connect(r'/Users/arlong/Projects/C_class/contact/contacts.db')

# 創建一個游標
cursor = conn.cursor()

# 執行查詢
cursor.execute("SELECT * FROM Contacts")

# 獲取所有結果
rows = cursor.fetchall()

# 打印結果
for row in rows:
    print(row)

# 關閉連接
conn.close()