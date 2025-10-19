from flask import Flask, request, jsonify, render_template
from datetime import datetime
import json

app = Flask(__name__)
door_history = []

@app.route('/api/door-status', methods=['POST'])
def receive_door_status():
    try:
        data = request.get_json()
        data['timestamp'] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        door_history.append(data)
        if len(door_history) > 100:
            door_history.pop(0)
        print(f"[{data['timestamp']}] Kapi durumu: {data['durum']}")
        with open('door_log.json', 'a', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False)
            f.write('\n')
        return jsonify({"status": "success", "message": "Veri alindi"}), 200
    except Exception as e:
        print(f"Hata: {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 400

@app.route('/api/door-history', methods=['GET'])
def get_door_history():
    return jsonify(door_history), 200

@app.route('/api/door-current', methods=['GET'])
def get_current_status():
    if door_history:
        return jsonify(door_history[-1]), 200
    else:
        return jsonify({"message": "Henuz veri yok"}), 404

@app.route('/')
def home():
    return render_template('index.html')

if __name__ == '__main__':
    print("=" * 50)
    print("KAPI DURUMU IZLEME SUNUCUSU")
    print("=" * 50)
    print("Web arayuzu: http://localhost:5000")
    print("API endpoint: http://localhost:5000/api/door-status")
    print("Sunucu calisiyor... Durdurmak icin CTRL+C")
    print("=" * 50)
    app.run(host='0.0.0.0', port=5000, debug=True)
