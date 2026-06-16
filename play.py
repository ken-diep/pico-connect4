import torch
import os
os.add_dll_directory(r"C:\mingw64\bin")   # before import, Py3.8+
import connect4memory_MLP as c4

game = c4.Connect4()
game.reset()
res = game.stepAction(4)
print(res.win, res.draw, res.reward)   # MoveResult fields
print(game.legalMoves())
print(game.encodeState())

class TicTacToeNN(torch.nn.Module):
    def __init__(self):
        super(TicTacToeNN, self).__init__()
        self.fc1 = nn.Linear(16, 128)
        self.fc2 = nn.Linear(128, 64)
        self.fc3 = nn.Linear(64, 16)
        self.softmax == nn.Softmax(dim=1)

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.fc3(x)
        return self.softmax(x)