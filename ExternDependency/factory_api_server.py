from flask import Flask, jsonify, request
from flask_cors import CORS
import random
import time
from datetime import datetime
from typing import Dict, List

app = Flask(__name__)
CORS(app)  # CORS 허용

# 시뮬레이션 데이터 저장
factory_data = {
    "cells": [
        {
            "actor_id": "BP_FactoryCell_Assembly_0",
            "production_progress": 0.0,
            "battery_level": -1,
            "efficiency": 1.0,
            "wear_level": 0.0,
            "cycle_count": 0
        },
        {
            "actor_id": "BP_FactoryCell_Assembly_1",
            "production_progress": 0.0,
            "battery_level": -1,
            "efficiency": 0.95,
            "wear_level": 0.15,
            "cycle_count": 150
        }
    ],
    "agvs": [
        {
            "actor_id": "BP_AGV_1",
            "production_progress": -1,
            "battery_level": 100.0,
            "current_load": 0.0,
            "status": "idle"
        },
        {
            "actor_id": "BP_AGV_2",
            "production_progress": -1,
            "battery_level": 87.5,
            "current_load": 1.0,
            "status": "moving"
        }
    ],
    "robots": [
        {
            "actor_id": "BP_CollabRobot_Welder_0",
            "production_progress": -1,
            "battery_level": -1,
            "status": "working"
        }
    ]
}

# 시뮬레이션 상태
simulation_enabled = True
last_update_time = time.time()


def simulate_factory_operations():
    """공장 작동 시뮬레이션"""
    global factory_data
    
    current_time = time.time()
    
    # Cell 생산 진행률 시뮬레이션
    for cell in factory_data["cells"]:
        if cell["production_progress"] < 100:
            # 효율에 따라 진행률 증가
            increment = random.uniform(1.0, 3.0) * cell["efficiency"]
            cell["production_progress"] = min(
                cell["production_progress"] + increment, 
                100.0
            )
            
            # 사이클 카운트 증가
            if cell["production_progress"] >= 100:
                cell["cycle_count"] += 1
                cell["production_progress"] = 0.0
                
                # 마모도 증가
                cell["wear_level"] = min(
                    cell["wear_level"] + random.uniform(0.01, 0.03),
                    1.0
                )
        else:
            cell["production_progress"] = 0.0
    
    # AGV 배터리 시뮬레이션
    for agv in factory_data["agvs"]:
        if agv["status"] == "moving":
            # 이동 중 배터리 소모
            agv["battery_level"] = max(
                agv["battery_level"] - random.uniform(0.1, 0.3),
                0.0
            )
            
            # 목적지 도착 시뮬레이션
            if random.random() < 0.1:
                agv["status"] = "idle"
                agv["current_load"] = 0.0
        elif agv["status"] == "idle":
            # 유휴 시 배터리 회복 (충전소에 있다고 가정)
            if agv["battery_level"] < 100:
                agv["battery_level"] = min(
                    agv["battery_level"] + random.uniform(0.5, 1.0),
                    100.0
                )
            
            # 새로운 작업 할당 시뮬레이션
            if random.random() < 0.05:
                agv["status"] = "moving"
                agv["current_load"] = 1.0


@app.route('/factory/status', methods=['GET'])
def get_factory_status():
    """
    공장 전체 상태 반환
    
    Returns:
        JSON: 모든 액터의 현재 상태
    """
    if simulation_enabled:
        simulate_factory_operations()
    
    # Unreal Engine이 기대하는 포맷으로 변환
    response_data = []
    
    for cell in factory_data["cells"]:
        response_data.append({
            "actor_id": cell["actor_id"],
            "production_progress": round(cell["production_progress"], 2),
            "battery_level": cell["battery_level"]
        })
    
    for agv in factory_data["agvs"]:
        response_data.append({
            "actor_id": agv["actor_id"],
            "production_progress": agv["production_progress"],
            "battery_level": round(agv["battery_level"], 2)
        })
    
    return jsonify({
        "timestamp": datetime.now().isoformat(),
        "data": response_data
    })


@app.route('/factory/cell/<cell_id>', methods=['GET'])
def get_cell_status(cell_id: str):
    """
    특정 Cell의 상태 반환
    
    Args:
        cell_id: Cell 액터 ID
    """
    for cell in factory_data["cells"]:
        if cell["actor_id"] == cell_id:
            return jsonify({
                "status": "success",
                "data": cell
            })
    
    return jsonify({
        "status": "error",
        "message": f"Cell {cell_id} not found"
    }), 404


@app.route('/factory/agv/<agv_id>', methods=['GET'])
def get_agv_status(agv_id: str):
    """
    특정 AGV의 상태 반환
    
    Args:
        agv_id: AGV 액터 ID
    """
    for agv in factory_data["agvs"]:
        if agv["actor_id"] == agv_id:
            return jsonify({
                "status": "success",
                "data": agv
            })
    
    return jsonify({
        "status": "error",
        "message": f"AGV {agv_id} not found"
    }), 404


@app.route('/factory/maintenance', methods=['GET'])
def get_maintenance_required():
    """
    유지보수가 필요한 설비 목록 반환
    """
    maintenance_list = []
    
    for cell in factory_data["cells"]:
        if cell["wear_level"] > 0.8:
            maintenance_list.append({
                "actor_id": cell["actor_id"],
                "wear_level": cell["wear_level"],
                "cycle_count": cell["cycle_count"],
                "priority": "high" if cell["wear_level"] > 0.9 else "medium"
            })
    
    return jsonify({
        "status": "success",
        "maintenance_required": len(maintenance_list) > 0,
        "items": maintenance_list
    })


@app.route('/factory/command/reset', methods=['POST'])
def reset_factory():
    """
    공장 상태 초기화
    """
    global factory_data
    
    # 모든 Cell 초기화
    for cell in factory_data["cells"]:
        cell["production_progress"] = 0.0
        cell["wear_level"] = 0.0
        cell["cycle_count"] = 0
        cell["efficiency"] = 1.0
    
    # 모든 AGV 초기화
    for agv in factory_data["agvs"]:
        agv["battery_level"] = 100.0
        agv["current_load"] = 0.0
        agv["status"] = "idle"
    
    return jsonify({
        "status": "success",
        "message": "Factory reset completed"
    })


@app.route('/factory/command/maintenance/<actor_id>', methods=['POST'])
def perform_maintenance(actor_id: str):
    """
    특정 액터의 유지보수 수행
    
    Args:
        actor_id: 유지보수할 액터 ID
    """
    for cell in factory_data["cells"]:
        if cell["actor_id"] == actor_id:
            cell["wear_level"] = 0.0
            cell["cycle_count"] = 0
            cell["efficiency"] = 1.0
            
            return jsonify({
                "status": "success",
                "message": f"Maintenance completed for {actor_id}"
            })
    
    return jsonify({
        "status": "error",
        "message": f"Actor {actor_id} not found"
    }), 404


@app.route('/factory/simulation/toggle', methods=['POST'])
def toggle_simulation():
    """
    시뮬레이션 활성화/비활성화
    """
    global simulation_enabled
    simulation_enabled = not simulation_enabled
    
    return jsonify({
        "status": "success",
        "simulation_enabled": simulation_enabled
    })


@app.route('/factory/config', methods=['GET'])
def get_config():
    """
    현재 설정 반환
    """
    return jsonify({
        "simulation_enabled": simulation_enabled,
        "total_cells": len(factory_data["cells"]),
        "total_agvs": len(factory_data["agvs"]),
        "total_robots": len(factory_data["robots"]),
        "api_version": "1.0.0"
    })


@app.route('/health', methods=['GET'])
def health_check():
    """
    서버 상태 체크
    """
    return jsonify({
        "status": "healthy",
        "timestamp": datetime.now().isoformat(),
        "uptime": time.time() - last_update_time
    })


@app.errorhandler(404)
def not_found(error):
    return jsonify({
        "status": "error",
        "message": "Endpoint not found"
    }), 404


@app.errorhandler(500)
def internal_error(error):
    return jsonify({
        "status": "error",
        "message": "Internal server error"
    }), 500


if __name__ == '__main__':
    print("=" * 50)
    print("Digital Twin Factory API Server")
    print("=" * 50)
    print(f"Server starting at http://127.0.0.1:8000")
    print("\nAvailable endpoints:")
    print("  GET  /factory/status              - 전체 공장 상태")
    print("  GET  /factory/cell/<id>           - 특정 Cell 상태")
    print("  GET  /factory/agv/<id>            - 특정 AGV 상태")
    print("  GET  /factory/maintenance         - 유지보수 필요 목록")
    print("  POST /factory/command/reset       - 공장 초기화")
    print("  POST /factory/command/maintenance/<id> - 유지보수 수행")
    print("  POST /factory/simulation/toggle   - 시뮬레이션 토글")
    print("  GET  /factory/config              - 설정 확인")
    print("  GET  /health                      - 서버 상태")
    print("\nPress Ctrl+C to stop the server")
    print("=" * 50)
    
    app.run(
        host='127.0.0.1',
        port=8000,
        debug=True,
        threaded=True
    )