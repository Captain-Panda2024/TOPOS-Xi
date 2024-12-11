import numpy as np
from topos import *
import math

# 物理定数
GRAVITY = 9.81  # m/s^2
SPHERE_DIAMETER = 0.02  # 2cm in meters
STRING_LENGTH = 0.2  # 20cm in meters

# 材質の物理特性
MATERIALS = {
    'iron': {
        'density': 7874,  # kg/m^3
        'color': (0.7, 0.7, 0.7),
    },
    'plastic': {
        'density': 1200,  # kg/m^3
        'color': (1.0, 0.5, 0.5),
    },
    'wood': {
        'density': 700,  # kg/m^3
        'color': (0.8, 0.6, 0.4),
    }
}

class PendulumSimulation:
    def __init__(self):
        # TOPOSシーンの初期化
        self.scene = Scene()
        self.setup_pendulums()
        
    def setup_pendulums(self):
        # 5つの振り子を横一列に配置
        materials = ['iron', 'plastic', 'wood', 'iron', 'plastic']
        spacing = SPHERE_DIAMETER * 3  # 球体間の間隔
        
        for i, material in enumerate(materials):
            # 振り子の初期位置を計算
            x_pos = (i - 2) * spacing  # 中心を0として左右に配置
            
            # 球体の体積とその材質の密度から質量を計算
            volume = (4/3) * math.pi * (SPHERE_DIAMETER / 2) ** 3
            mass = volume * MATERIALS[material]['density']
            
            # 球体オブジェクトの作成
            sphere = Sphere(
                radius=SPHERE_DIAMETER / 2,
                mass=mass,
                position=Vector3(x_pos, -STRING_LENGTH, 0),
                color=MATERIALS[material]['color']
            )
            
            # 固定点（振り子の支点）の作成
            anchor = Point(Vector3(x_pos, 0, 0))
            
            # 振り子の紐（コンストレイント）の作成
            string = DistanceConstraint(anchor, sphere, STRING_LENGTH)
            
            # シーンにオブジェクトを追加
            self.scene.add_object(sphere)
            self.scene.add_constraint(string)
            
            # 初期速度を与える（オプション）
            sphere.velocity = Vector3(0.1, 0, 0)

    def update(self, dt):
        # 物理シミュレーションの更新
        self.scene.step(dt)
        
        # 衝突検出と処理
        self.check_collisions()
        
    def check_collisions(self):
        objects = self.scene.get_objects()
        for i in range(len(objects)):
            for j in range(i + 1, len(objects)):
                if isinstance(objects[i], Sphere) and isinstance(objects[j], Sphere):
                    self.handle_sphere_collision(objects[i], objects[j])
    
    def handle_sphere_collision(self, sphere1, sphere2):
        # 球体間の距離を計算
        distance = (sphere1.position - sphere2.position).length()
        min_distance = SPHERE_DIAMETER
        
        if distance < min_distance:
            # 衝突応答の計算
            normal = (sphere2.position - sphere1.position).normalized()
            relative_velocity = sphere2.velocity - sphere1.velocity
            
            # 反発係数（0.8は適度な弾性衝突を表現）
            restitution = 0.8
            
            # 運動量保存に基づく速度更新
            total_mass = sphere1.mass + sphere2.mass
            impulse = -(1 + restitution) * relative_velocity.dot(normal)
            impulse = impulse / total_mass
            
            sphere1.velocity -= impulse * sphere2.mass * normal
            sphere2.velocity += impulse * sphere1.mass * normal
            
            # めり込み防止
            overlap = min_distance - distance
            sphere1.position -= overlap * 0.5 * normal
            sphere2.position += overlap * 0.5 * normal

    def run(self):
        # メインループ
        time_step = 1/60  # 60FPS
        running = True
        while running:
            self.update(time_step)
            
            # シーンの状態を表示
            self.display_state()
            
            # 簡易的な状態表示（後でグラフィカルな表示に置き換え可能）
            print("\033[2J\033[H")  # 画面クリア
            for obj in self.scene.get_objects():
                if isinstance(obj, Sphere):
                    print(f"Sphere at position: {obj.position}, velocity: {obj.velocity}")
            
    def display_state(self):
        # 後でグラフィカルな表示に置き換えるためのプレースホルダー
        pass

def select_material(i):
    materials = ['iron', 'plastic', 'wood', 'iron', 'plastic']
    return MATERIALS[materials[i + 2]]

def calculate_gravity_force(pendulum):
    return Vector3(0, -GRAVITY, 0)

def apply_distance_constraint(pendulum):
    anchor_point = Vector3(pendulum.position.x, 0, 0)
    distance = (pendulum.position - anchor_point).length()
    if distance > STRING_LENGTH:
        pendulum.position = anchor_point + (pendulum.position - anchor_point).normalized() * STRING_LENGTH

if __name__ == "__main__":
    simulation = PendulumSimulation()
    simulation.run()
