import torch
import torch.nn as nn
import os
os.add_dll_directory(r"C:\mingw64\bin")   # before import, Py3.8+
import connect4memory_MLP as c4
import random
from visualizer import Visualizer

#declarations
history_global = []
games_played = 0
class Connect4NN(nn.Module):
    def __init__(self):
        super(Connect4NN, self).__init__()
        self.fc1 = nn.Linear(16, 128)
        self.fc2 = nn.Linear(128, 64)
        self.fc3 = nn.Linear(64, 16)
        #self.softmax == nn.Softmax(dim=1)
        self.optimizer = torch.optim.Adam(self.parameters(), lr=1e-3)

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.fc3(x)
        return x
        #return self.softmax(x)

    def calculate_return(self, winner, history):
        if winner == 'X':
            for index, item in enumerate(history):   #every even index is X, odd is O
                if not (index % 2):
                    history[index][3] = 1 #return for winning move
                else:
                    history[index][3] = -1
        elif winner == 'O':
            for index, item in enumerate(history):
                if (index % 2):
                    history[index][3] = 1 #return for winning move
                else:
                    history[index][3] = -1

        return history

    def backward(self, history):        
        reward_stacked = torch.tensor([item[3] for item in history], 
                                      dtype=torch.float32)
        log_prob_stacked = torch.stack([item[2] for item in history])
            # reward_log_prob_mul = torch.mul(reward_stacked, log_prob_stacked)
            # reward_log_prob_sum = torch.sum(reward_log_prob_mul)
        loss = -(log_prob_stacked * reward_stacked).sum()
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()
        return loss.item()
    
    def state_to_tensor(self, x):
        return torch.tensor(x, dtype=torch.float32).unsqueeze(0)

def runNN():
    global history_global

    state = game.encodeState()
    state_tensor = net.state_to_tensor(state)
    logits = net.forward(state_tensor)

    moves = game.legalMoves()
    mask = torch.zeros(16)
    for index in range(0, 16):
        if index not in moves:
            mask[index] = float('-inf')

    masked_logits = logits + mask
    m = torch.distributions.Categorical(logits=masked_logits)
    action = m.sample()
    log_prob = m.log_prob(action)
    move = action.item()
    res = game.stepAction(move)

    history_global.append([state_tensor, action, log_prob, 0])
    return move

def step(viz):
    global history_global, games_played
    if game.isGameOver():
        w = game.checkWinner()
        viz.add_score(w if w in ("X", "O") else "Draw")

        history = net.calculate_return(game.checkWinner(), history_global)
        net.backward(history)
        history = []
        history_global = []
        game.reset()
        viz.clear()
        games_played+=1
        return history
    
    move = runNN()
    grid = game.getGrid()
    #grid = [grid[i*4:(i+1)*4] for i in range(4)]
    grid = [[None if c == '-' else c for c in grid[i*4:(i+1)*4]] for i in range(4)]
    
    viz.draw_grid(grid)
    viz.set_turnnumber(game.getTurnNumber())
    #viz.set_cell(move // 4, move % 4, game.currentPlayer())

def viz_reset(viz):
    game.reset()
    torch.save(net.state_dict(), f"connect4_gp{games_played}.pt")

def greedy_move(net, game):
    state = net.state_to_tensor(game.encodeState())
    with torch.no_grad():                      # no grad in eval
        logits = net.forward(state)
    moves = game.legalMoves()
    mask = torch.full((16,), float('-inf'))
    mask[moves] = 0.0                          # legal = 0, illegal = -inf
    masked = logits + mask
    return int(masked.argmax().item())         # best legal move

def eval_vs_random(net, n=100, net_plays='X'):
    net.eval()
    wins = draws = losses = 0
    g = c4.Connect4()                          # own game, don't touch training game
    for _ in range(n):
        g.reset()
        while not g.isGameOver():
            if g.currentPlayer() == net_plays:
                mv = greedy_move(net, g)
            else:
                mv = random.choice(g.legalMoves())
            g.stepAction(mv)
        w = g.checkWinner()
        if w == net_plays:        wins += 1
        elif w in ("X", "O"):     losses += 1
        else:                     draws += 1
    net.train()                                # back to train mode
    return wins / n, draws / n, losses / n


game = c4.Connect4()
game.reset()
#res = game.stepAction(4)
#print(res.win, res.draw, res.reward)   # MoveResult fields
net = Connect4NN()
net.load_state_dict(torch.load("connect4_gp714.pt"))
win, draw, loss = eval_vs_random(net, 200)
print(f"win={win:.2f} draw={draw:.2f} loss={loss:.2f}")

#viz = Visualizer(size=4, on_step=step, on_reset=viz_reset)
#viz.run()    

#pick a move from the list
#while not game.isGameOver():

#if win or draw, restart
#loss.backward()     
    
# class Connect4NN_Dataset(Dataset):
#     def __init__(self, boards, moves):
#         self.boards = boards
#         self.moves = moves