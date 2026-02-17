
import socket
import struct
import random
import time
import pickle
import os

# Configuration
HOST = '127.0.0.1'
PORT = 54000
ALPHA = 0.1   # LR
GAMMA = 0.99  
EPSILON = 0.5 # High exploration initially
SAVE_FILE = "q_table.pkl"

class FlappyAgent:
    def __init__(self):
        self.q_table = {}
        self.load_q_table()
        self.last_state = None
        self.last_action = 0
        self.high_score = 0
        self.games_played = 0

    def load_q_table(self):
        if os.path.exists(SAVE_FILE):
            with open(SAVE_FILE, "rb") as f:
                self.q_table = pickle.load(f)
            print(f"Loaded Q-table with {len(self.q_table)} states.")
        else:
            print("Starting with new Q-table.")

    def save_q_table(self):
        with open(SAVE_FILE, "wb") as f:
            pickle.dump(self.q_table, f)
        print("Q-table saved.")

    def get_state(self, bird_y, bird_vel, pipe_x, pipe_y):
        # Discretize state (Coarser buckets = fewer states = faster learning)
        
        if pipe_x == -1: 
             dist_x = 5 
             dist_y = 0
        else:
            dist_x = int(pipe_x / 140) # Fewer horizontal states (0, 1, 2, 3)
            
            gap_center = pipe_y - 85
            diff_y = bird_y - gap_center
            dist_y = int(diff_y / 60) # Fewer vertical states (-5 to +5)

        vel_y = int(bird_vel / 80) # Fewer velocity states (-3 to +3)
        
        return (dist_x, dist_y, vel_y)

    def choose_action(self, state):
        if random.random() < EPSILON:
            # Force explore
            return random.choice([0, 1])
        
        q0 = self.q_table.get((state, 0), 0.0)
        q1 = self.q_table.get((state, 1), 0.0)
        
        if q1 > q0:
            return 1
        return 0

    def update_q_table(self, state, action, reward, next_state):
        current_q = self.q_table.get((state, action), 0.0)
        
        # Max Q for next state
        q0_next = self.q_table.get((next_state, 0), 0.0)
        q1_next = self.q_table.get((next_state, 1), 0.0)
        max_next_q = max(q0_next, q1_next)
        
        new_q = current_q + ALPHA * (reward + GAMMA * max_next_q - current_q)
        self.q_table[(state, action)] = new_q

    def run(self):
        print(f"Connecting to {HOST}:{PORT}...")
        while True:
            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.connect((HOST, PORT))
                    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1) # Disable Nagle in Python too
                    print("Connected!")
                    
                    self.last_state = None
                    self.last_action = 0
                    
                    while True:
                        # Ensure we read exactly 24 bytes
                        data = b''
                        while len(data) < 24:
                            packet = s.recv(24 - len(data))
                            if not packet:
                                break
                            data += packet
                            
                        if len(data) < 24:
                            break
                        
                        bird_y, bird_vel, pipe_x, pipe_y, is_alive, score = struct.unpack('ffffii', data)
                        
                        state = self.get_state(bird_y, bird_vel, pipe_x, pipe_y)
                        
                        reward = 0
                        if is_alive:
                            reward = 1 # Survival reward
                            if score > self.high_score:
                                reward = 10 # Bonus for score
                                self.high_score = score
                        else:
                            reward = -1000 # Death penalty
                        
                        # Train from previous step
                        if self.last_state is not None:
                            self.update_q_table(self.last_state, self.last_action, reward, state)
                        
                        if not is_alive:
                            self.last_state = None # Reset for next game
                            # Send ack (0)
                            s.sendall(struct.pack('i', 0))
                            self.games_played += 1
                            if self.games_played % 10 == 0:
                                self.save_q_table()
                                print(f"Games: {self.games_played}, High Score: {self.high_score}, Epsilon: {EPSILON:.2f}")
                            continue

                        # Choose Action
                        action = self.choose_action(state)
                        self.last_state = state
                        self.last_action = action
                        
                        # Debug Prints
                        # print(f"State: {state} | Action: {'JUMP' if action == 1 else 'Wait'}")
                        
                        if action == 1:
                            pass

                        # Send Action
                        s.sendall(struct.pack('i', action))
                        
            except (ConnectionRefusedError, ConnectionResetError):
                print("Connection lost. Retrying in 2s...")
                time.sleep(2)
            except Exception as e:
                print(f"Error: {e}")
                time.sleep(2)

if __name__ == "__main__":
    agent = FlappyAgent()
    # To train from scratch, uncomment below or handle externally:
    EPSILON = 0.1 # Some exploration
    agent.run()
