# ESP32 Coordinate Dashboard Project

## Purpose
This is a **backend + frontend + MCU (ESP32-WROOM-32U)** project.  

- The ESP sends **x,y coordinates**.  
- The backend **catches** them.  
- The frontend **displays** them.  

The MCU implementation is made for **two IDEs**:  
- **Arduino IDE**  
- **VSCode + ESP-IDF (Extension)**  

The project can be built in **two ways**:  
1. **Separate solution** – backend and frontend are separate.  
2. **Single solution** – everything inside a single **Next.js** app.  

---

## 🔹 Separate Solution

### Backend Setup
```bash
mkdir project
cd project
mkdir backend
cd backend

npm init -y   # Creates package.json with default values
npm install ws   # Installs the ws package used for the WebSocket server

code tcp-server.js   # Opens file in VSCode
# If 'code' is not recognized, open VSCode > Command Palette > 
# "Shell Command: Install 'code' command in PATH"
```

Write the backend code in `tcp-server.js`, then run:
```bash
node tcp-server.js
```

---

### Frontend Setup
```bash
mkdir frontend
cd frontend
npm init -y
```

*(Continue with your frontend implementation here)*

---

## 🔹 Single Solution (Next.js)

### Create Next.js App
```bash
mkdir project
cd project
npx create-next-app@latest project
```

Answer the prompts as follows:
```
√ Would you like to use TypeScript? ... No
√ Would you like to use ESLint? ... No
√ Would you like to use Tailwind CSS? ... Yes
√ Would you like your code inside a `src/` directory? ... No
√ Would you like to use App Router? ... No
√ Would you like to use Turbopack for `next dev`? ... No
√ Would you like to customize the import alias? ... No
```

Then install WebSocket:
```bash
cd project
npm install ws
```

---

### Update `package.json`
Make sure your **package.json** looks like this:
```json
{
  "name": "live-dashboard",
  "version": "0.1.0",
  "private": true,
  "scripts": {
    "dev": "node server.js",
    "build": "next build",
    "start": "NODE_ENV=production node server.js",
    "lint": "next lint"
  },
  "dependencies": {
    "react": "^19.0.0",
    "react-dom": "^19.0.0",
    "next": "15.3.5"
  },
  "devDependencies": {
    "@tailwindcss/postcss": "^4",
    "tailwindcss": "^4"
  }
}
```

Write the client and server-side code in their respective folders, then start the project:
```bash
npm run dev
```

---

## 🚀 Running the Project

### Backend
```bash
cd backend
npm install ws
node tcp-server.js
```

### Frontend
```bash
cd frontend
npm install
npm run dev
```

---

## 🔹 MCU Code

1. Open this folder in VSCode:
   ```
   D:\Work\ProjectsForYourself\Project1\app4\Mcu\ESP-IDF – Micropython\mcu
   ```
2. In **ESP-IDF Explorer**, **flash the device**.

---

## 🔹 Simulating MCU Code

You can simulate ESP32 coordinate sending via `ncat`:
```bash
echo 10,20 | ncat localhost 5000
echo 30,50 | ncat localhost 5000
echo 100,200 | ncat localhost 5000
```
---

✅ Now you’re ready to run both **separate** and **single (Next.js)** solutions!
