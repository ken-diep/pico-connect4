"""Minimal 4x4 board rendering framework.

No game logic lives here - you plug in your own. The Visualizer just draws a
grid (X = red cross, O = blue circle) and gives you Play/Pause + Reset buttons
and a scoreboard.

Wire it up by passing two callbacks:
    on_step  - called repeatedly (on a timer) while "Play" is active. Do one
               move of your game here and update the board.
    on_reset - called when "Reset" is pressed. Reset your game state here.

Render with:
    viz.set_cell(row, col, "X" | "O" | None)
    viz.draw_grid(grid)          # grid = 4x4 list of "X"/"O"/None
    viz.clear()
    viz.add_score("X" | "O" | "Draw")
    viz.set_scores(x=, o=, draw=)
    viz.set_status("text")
    viz.set_turnnumber(int)

See the bottom of the file for a play.py-style example.
"""

import tkinter as tk

CELL = 110             # pixel size of one board cell
PAD = 18               # inner padding for drawn markers
STEP_DELAY_MS = 10    # time between on_step calls while playing

BG = "#202830"
GRID_BG = "#2b3542"
LINE = "#46566b"
X_COLOR = "#e23b3b"    # red for X
O_COLOR = "#3b7be2"    # blue for O


class Visualizer:
    def __init__(self, size=4, on_step=None, on_reset=None, title="Connect 4"):
        self.size = size
        self.on_step = on_step
        self.on_reset = on_reset

        self.board = [[None] * size for _ in range(size)]
        self.scores = {"X": 0, "O": 0, "Draw": 0}
        self.turn_number = 0
        self.playing = False
        self._after_id = None

        self.root = tk.Tk()
        self.root.title(title)
        self.root.configure(bg=BG)
        self.root.resizable(False, False)
        self._build_ui()
        self._redraw()

    # -- public rendering API ----------------------------------------------
    def set_cell(self, row, col, marker):
        """marker: 'X', 'O', or None."""
        self.board[row][col] = marker
        self._redraw()

    def draw_grid(self, grid):
        """grid: size x size list of 'X'/'O'/None."""
        self.board = [list(r) for r in grid]
        self._redraw()

    def clear(self):
        self.board = [[None] * self.size for _ in range(self.size)]
        self._redraw()

    def add_score(self, key):
        """key: 'X', 'O', or 'Draw'."""
        self.scores[key] += 1
        self._update_scoreboard()

    def set_scores(self, x=None, o=None, draw=None):
        if x is not None:
            self.scores["X"] = x
        if o is not None:
            self.scores["O"] = o
        if draw is not None:
            self.scores["Draw"] = draw
        self._update_scoreboard()

    def set_status(self, text):
        self.status.config(text=text)

    def set_turnnumber(self, n):
        self.turn_number = n
        self._update_turn_label()

    def run(self):
        self.root.mainloop()

    # -- UI construction ----------------------------------------------------
    def _build_ui(self):
        board_px = CELL * self.size
        self.canvas = tk.Canvas(
            self.root, width=board_px, height=board_px,
            bg=GRID_BG, highlightthickness=0,
        )
        self.canvas.pack(padx=20, pady=(20, 8))

        self.status = tk.Label(
            self.root, text="Ready", font=("Segoe UI", 11),
            fg="#aab6c4", bg=BG,
        )
        self.status.pack(pady=4)

        self.turn_label = tk.Label(
            self.root, font=("Segoe UI", 11), fg="#e8edf2", bg=BG,
        )
        self.turn_label.pack(pady=2)
        self._update_turn_label()

        score_frame = tk.Frame(self.root, bg=BG)
        score_frame.pack(pady=4)
        self.x_label = tk.Label(score_frame, font=("Segoe UI", 12, "bold"),
                                fg=X_COLOR, bg=BG)
        self.x_label.grid(row=0, column=0, padx=16)
        self.draw_label = tk.Label(score_frame, font=("Segoe UI", 12, "bold"),
                                   fg="#aab6c4", bg=BG)
        self.draw_label.grid(row=0, column=1, padx=16)
        self.o_label = tk.Label(score_frame, font=("Segoe UI", 12, "bold"),
                                fg=O_COLOR, bg=BG)
        self.o_label.grid(row=0, column=2, padx=16)
        self._update_scoreboard()

        btn_frame = tk.Frame(self.root, bg=BG)
        btn_frame.pack(pady=(8, 16))
        self.play_btn = tk.Button(btn_frame, text="Play", width=10,
                                  font=("Segoe UI", 11), command=self._toggle_play)
        self.play_btn.grid(row=0, column=0, padx=8)
        tk.Button(btn_frame, text="Reset", width=10, font=("Segoe UI", 11),
                  command=self._reset).grid(row=0, column=1, padx=8)

    # -- drawing ------------------------------------------------------------
    def _redraw(self):
        self.canvas.delete("all")
        n = self.size
        for i in range(1, n):
            self.canvas.create_line(i * CELL, 0, i * CELL, CELL * n, fill=LINE)
            self.canvas.create_line(0, i * CELL, CELL * n, i * CELL, fill=LINE)

        for row in range(n):
            for col in range(n):
                marker = self.board[row][col]
                if marker is None:
                    continue
                x0, y0 = col * CELL + PAD, row * CELL + PAD
                x1, y1 = (col + 1) * CELL - PAD, (row + 1) * CELL - PAD
                if marker == "X":
                    self.canvas.create_line(x0, y0, x1, y1, fill=X_COLOR,
                                            width=8, capstyle=tk.ROUND)
                    self.canvas.create_line(x0, y1, x1, y0, fill=X_COLOR,
                                            width=8, capstyle=tk.ROUND)
                else:  # O
                    self.canvas.create_oval(x0, y0, x1, y1,
                                            outline=O_COLOR, width=8)

    def _update_scoreboard(self):
        self.x_label.config(text=f"X wins: {self.scores['X']}")
        self.draw_label.config(text=f"Draws: {self.scores['Draw']}")
        self.o_label.config(text=f"O wins: {self.scores['O']}")

    def _update_turn_label(self):
        self.turn_label.config(text=f"Move: {self.turn_number}")

    # -- button handling ----------------------------------------------------
    def _toggle_play(self):
        if self.playing:
            self.playing = False
            self.play_btn.config(text="Play")
            if self._after_id is not None:
                self.root.after_cancel(self._after_id)
                self._after_id = None
        else:
            self.playing = True
            self.play_btn.config(text="Pause")
            self._tick()

    def _tick(self):
        if not self.playing:
            return
        if self.on_step is not None:
            self.on_step(self)
        self._after_id = self.root.after(STEP_DELAY_MS, self._tick)

    def _reset(self):
        self.playing = False
        self.play_btn.config(text="Play")
        if self._after_id is not None:
            self.root.after_cancel(self._after_id)
            self._after_id = None
        self.clear()
        if self.on_reset is not None:
            self.on_reset(self)


# ---------------------------------------------------------------------------
# Example: this is roughly how you'd drive it from play.py. Replace the body
# of `step` with your own move selection.
if __name__ == "__main__":
    import random

    def step(viz):
        # demo only: drop a random marker on an empty cell
        empties = [(r, c) for r in range(4) for c in range(4)
                   if viz.board[r][c] is None]
        if not empties:
            viz.add_score(random.choice(["X", "O"]))
            viz.clear()
            return
        r, c = random.choice(empties)
        viz.set_cell(r, c, random.choice(["X", "O"]))

    def reset(viz):
        viz.set_status("Reset")

    viz = Visualizer(size=4, on_step=step, on_reset=reset)
    viz.run()
