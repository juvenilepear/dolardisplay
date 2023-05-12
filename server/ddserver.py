# ddserver.py
from flask import Flask, request, jsonify

app = Flask(__name__)

sources = [
    {"id": 1, "name": "BCV", "tasa": 24.25},
    {"id": 2, "name": "MonitorDolar", "tasa": 25.25},
    {"id": 3, "name": "CLPtoday", "tasa": 200.20},
]

@app.get("/sources")
def get_sources():
    return jsonify(sources)

@app.route("/update", methods=["POST"])
def save_config():
    print(request.headers)
    data = request.json
    sources[0]["tasa"] = data[0]["tasa"]
    sources[1]["tasa"] = data[1]["tasa"]
    print(data)
    return jsonify(data)

