# sources.mk
# ----------
# Edit this file!

# +-- Set to 0 for English board output --+
CHINESE = 1

# +-- Add your own sources here, if any --+
ADD_SOURCES = mcts/cpp/mcts.cpp \
			  mcts/cpp/simulation.cpp \
			  alphabeta/cpp/alphabeta.cpp \
			  utils/cpp/zobrist.cpp \
			  utils/cpp/eval.cpp