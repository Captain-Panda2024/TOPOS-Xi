// ビリヤード（ナインボール）ファーストショットシミュレーション
// TOPOS-Ξ言語による物理シミュレーション

// 物理定数の定義
const float GRAVITY = 9.81;
const float FRICTION_COEFFICIENT = 0.2;
const float TABLE_LENGTH = 2.54; // メートル
const float TABLE_WIDTH = 1.27;  // メートル
const float BALL_RADIUS = 0.028575; // メートル
const float BALL_MASS = 0.170;   // キログラム

// ボールの状態を表す構造体
struct BallState {
    Vector2D position;
    Vector2D velocity;
    float angular_velocity;
    bool in_play;
}

// 2次元ベクトル
struct Vector2D {
    float x;
    float y;
}

// シミュレーション空間の定義
topology BilliardsSpace {
    dimension: 2;
    boundary: rectangular(TABLE_LENGTH, TABLE_WIDTH);
    
    // ボールの衝突判定
    collision_rule(ball1: BallState, ball2: BallState) {
        if (distance(ball1.position, ball2.position) <= 2 * BALL_RADIUS) {
            // 弾性衝突の計算
            Vector2D normal = normalize(ball2.position - ball1.position);
            Vector2D relative_velocity = ball2.velocity - ball1.velocity;
            
            float impulse = dot(relative_velocity, normal) * (1 + 0.95) / 2;
            
            ball1.velocity += normal * impulse;
            ball2.velocity -= normal * impulse;
            
            // 角運動量の保存
            ball1.angular_velocity = cross(ball1.velocity, normal) / BALL_RADIUS;
            ball2.angular_velocity = cross(ball2.velocity, normal) / BALL_RADIUS;
        }
    }
    
    // 時間発展の規則
    evolution_rule(dt: float) {
        for each ball in balls {
            if (ball.in_play) {
                // 速度による位置の更新
                ball.position += ball.velocity * dt;
                
                // 摩擦による減速
                float friction_force = FRICTION_COEFFICIENT * GRAVITY;
                Vector2D friction_acceleration = -normalize(ball.velocity) * friction_force;
                ball.velocity += friction_acceleration * dt;
                
                // 速度が十分小さくなった場合は停止
                if (magnitude(ball.velocity) < 0.01) {
                    ball.velocity = Vector2D(0, 0);
                    ball.angular_velocity = 0;
                }
                
                // クッションとの衝突判定
                if (ball.position.x - BALL_RADIUS < 0 || 
                    ball.position.x + BALL_RADIUS > TABLE_LENGTH) {
                    ball.velocity.x *= -0.95; // エネルギー損失を考慮
                }
                if (ball.position.y - BALL_RADIUS < 0 || 
                    ball.position.y + BALL_RADIUS > TABLE_WIDTH) {
                    ball.velocity.y *= -0.95;
                }
            }
        }
    }
}

// ファーストショットのセットアップ
function setup_first_shot() {
    // ボールの初期配置（ダイヤモンド形）
    BallState[9] balls;
    
    // 白球（手玉）の配置
    balls[0].position = Vector2D(TABLE_LENGTH * 0.25, TABLE_WIDTH / 2);
    balls[0].velocity = Vector2D(0, 0);
    balls[0].in_play = true;
    
    // ナインボールのラック配置
    float rack_x = TABLE_LENGTH * 0.75;
    float rack_y = TABLE_WIDTH / 2;
    float row_spacing = BALL_RADIUS * 2 * 0.87; // cos(30度)を考慮
    
    int ball_index = 1;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col <= row; col++) {
            if (ball_index < 9) {
                float x = rack_x + row * row_spacing;
                float y = rack_y + (col - row/2.0) * BALL_RADIUS * 2;
                balls[ball_index].position = Vector2D(x, y);
                balls[ball_index].velocity = Vector2D(0, 0);
                balls[ball_index].in_play = true;
                ball_index++;
            }
        }
    }
    
    return balls;
}

// メイン関数
function main() {
    // シミュレーション空間の初期化
    BilliardsSpace space;
    
    // ファーストショットのセットアップ
    BallState[9] balls = setup_first_shot();
    
    // 手玉に初速を与える（ブレイクショット）
    float shot_power = 10.0; // m/s
    float shot_angle = 0.0;  // ラジアン
    balls[0].velocity = Vector2D(
        shot_power * cos(shot_angle),
        shot_power * sin(shot_angle)
    );
    
    // シミュレーション実行
    float dt = 0.001; // 時間刻み
    float total_time = 0.0;
    float max_time = 10.0; // 最大シミュレーション時間
    
    while (total_time < max_time) {
        space.evolution_rule(dt);
        total_time += dt;
        
        // すべてのボールが停止したかチェック
        bool all_stopped = true;
        for (ball in balls) {
            if (magnitude(ball.velocity) > 0.01) {
                all_stopped = false;
                break;
            }
        }
        
        if (all_stopped) {
            break;
        }
    }
}
