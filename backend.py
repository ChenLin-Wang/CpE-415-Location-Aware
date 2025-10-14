from flask import Flask, jsonify, render_template
import sqlite3

app = Flask(__name__)
DB_PATH = "/tmp/sqlite"

def get_db_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/api/device_locations")
def get_locations():
    conn = get_db_connection()
    cursor = conn.execute("""
        SELECT dl.id, dl.device_id, dl.latitude, dl.longitude, dl.timestamp, d.name, d.description
        FROM device_locations dl
        JOIN devices d ON dl.device_id = d.id
        ORDER BY dl.timestamp DESC
    """)
    locations = [dict(row) for row in cursor.fetchall()]
    conn.close()
    return jsonify(locations)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=60000)