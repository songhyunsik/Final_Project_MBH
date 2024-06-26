import requests
import mysql.connector
from mysql.connector import Error

# API에서 데이터 받아오기
api_url = 'https://api.example.com/users'
response = requests.get(api_url)
users_data = response.json()

# MySQL에 연결
try:
    connection = mysql.connector.connect(
        host='localhost',
        database='Cloud',
        user='root',
        password='0000'
    )

    if connection.is_connected():
        cursor = connection.cursor()

        # 데이터를 MySQL에 삽입
        for user in users_data:
            name = user['name']
            email = user['email']
            
            insert_query = """INSERT INTO users (name, email) VALUES (%s, %s)"""
            record_tuple = (name, email)
            cursor.execute(insert_query, record_tuple)

        connection.commit()
        print("데이터가 성공적으로 삽입되었습니다.")

except Error as e:
    print(f"Error: {e}")

finally:
    if connection.is_connected():
        cursor.close()
        connection.close()
        print("MySQL 연결이 종료되었습니다.")
