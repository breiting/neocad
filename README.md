# neocad

A radically simple CAD system for makers, hackers & designers.

## 🚀 What is neocad?

neocad is a new kind of CAD system — built from scratch with one clear goal:

_Make CAD simple, lightweight, scriptable, and fun again._

Inspired by [Neovim](https://neovim.io), neocad blends UI interaction + scripting + parametric geometry into one coherent workflow — minimal, fast, and fully under your control.

## 🧠 Core Principles

- Simple & minimalistic — no clutter, no hidden magic.
- Fast & lightweight — focused on fundamentals.
- Inspired by Neovim — modal, keyboard-driven, scriptable.
- For makers, hobbyists, engineers — not corporate CAD departments.
- Parametric & reproducible — everything is data, everything can regenerate.
- Open-source.

## 🛠 How You Can Use neocad

You can design in three different ways — or mix them together:

| Mode            | Description                          |
| --------------- | ------------------------------------ |
| 🧠 Script-based | model.lua DSL → parametric geometry  |
| 🖱 UI-based     | 2D/3D sketching tools                |
| 🔁 Hybrid       | UI generates code, scripts modify UI |

✔ Design visually
✔ Automate with code
✔ Or merge both worlds seamlessly

## 🧬 What Makes neocad Different?

- Minimal & intuitive — no complexity paralysis
- ECS / node-graph based geometry engine
- Fully parametric — everything is repeatable
- Real-time geometry updates
- No file-format trap — your model is yours
- CAD + programming → finally unified
- Built as a tool, not a product

## 🎯 Who Is neocad For?

- 🚀 Makers & 3D printing enthusiasts
- 📦 Product designers
- 🛠 DIY & hobby engineers
- 🤖 Robotics & hardware developers
- 👨‍💻 Coders who love parametric control
- 🔥 Small teams who want freedom
- 👨‍🎨 Creative people who hate bloated software

## 🌌 Long-Term Vision

neocad aims to be:

“The Neovim of CAD.” A powerful, elegant, scriptable tool — where geometry becomes code, and CAD becomes creative & fun again.

## 📦 Dependencies

### Core Dependencies

- C++17
- CMake
- OpenGL / GLAD / GLFW
- GLM
- OCCT 7.9+ (OpenCascade — geometry kernel)
- Lua (scripting DSL)

## 🧱 Build Instructions

```
git clone https://github.com/breiting/neocad.git
cd neocad

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
./apps/neocad/neocad
```

## 🧭 Roadmap

neocad just got started and will grow.

- Basic architecture ✔
- Fast viewer ✔
- Basic sketching tools ✔
- Lua parametric DSL ✔
- OCCT-based STEP import/export ✔
- Constraint system
- 3D feature modeling (extrude / revolve / sweep)

## 🤝 Contribution

neocad is open-source — but it’s not a product. It’s a tool, and tools evolve with their users.

- 💡 Ideas?
- 🐛 Bugs?
- 🌟 Feature requests?

→ Open an issue or PR — let’s build this together.

## 🧪 Experimental Philosophy

Complex tools kill creativity. neocad is built for flow, speed, and clarity. Not for bureaucracy.

## 📜 License

neocad is open-source and released under the **MIT License** —  
you are free to use it for personal, educational and commercial projects.

## ⭐ Support the project

If you like this idea — star ⭐ the repo — it helps big time! More stars → more contributors → more speed.
