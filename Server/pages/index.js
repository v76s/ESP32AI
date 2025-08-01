import { useEffect, useRef, useState } from 'react';

export default function Home() {
  const [coordinates, setCoordinates] = useState([]);
  const canvasRef = useRef(null);

  const CANVAS_WIDTH = 800;
  const CANVAS_HEIGHT = 600;

  const getScaledCoordinates = (point) => {
    // Simple scaling to fit canvas size (assuming values 0-1000)
    const scaleX = CANVAS_WIDTH / 1000;
    const scaleY = CANVAS_HEIGHT / 1000;
    return { x: point.x * scaleX, y: point.y * scaleY };
  };

 useEffect(() => {
  // const socket = new WebSocket('ws://localhost:4000');
  const socket = new WebSocket(`ws://${window.location.hostname}:4000`);

  socket.onopen = () => console.log('WebSocket connected');
  socket.onerror = (err) => console.error('WebSocket error', err);
  socket.onclose = () => console.log('WebSocket closed');

  socket.onmessage = (event) => {
    console.log('Received from WS:', event.data);
    const [xStr, yStr] = event.data.split(',');
    const x = parseInt(xStr, 10);
    const y = parseInt(yStr, 10);
    setCoordinates(prev => [...prev, { x, y }]);
  };

  return () => socket.close();
}, []);

  useEffect(() => {
    const canvas = canvasRef.current;
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    ctx.beginPath();
    coordinates.forEach((point, index) => {
      const { x, y } = getScaledCoordinates(point);
      if (index === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }
    });
    ctx.strokeStyle = 'blue';
    ctx.lineWidth = 2;
    ctx.stroke();

    // Draw small circle for each point
    coordinates.forEach((point, index) => {
      const { x, y } = getScaledCoordinates(point);
      ctx.beginPath();
      ctx.arc(x, y, 4, 0, Math.PI * 2);
      ctx.fillStyle = index % 2 === 0 ? 'red' : 'green';
      ctx.fill();
    });
  }, [coordinates]);

  const clearCanvas = () => {
    setCoordinates([]);
  };

  return (
    <div style={{ 
      display: 'flex', 
      flexDirection: 'column', 
      alignItems: 'center', 
      justifyContent: 'center', 
      minHeight: '100vh', 
      padding: '1rem' 
    }}>
      <h1>Live Coordinates Line Viewer</h1>
      <canvas
        ref={canvasRef}
        width={CANVAS_WIDTH}
        height={CANVAS_HEIGHT}
        style={{ border: '1px solid black', marginBottom: '1rem' }}
      />
      <div>
        <button onClick={clearCanvas} style={{ padding: '0.5rem 1rem', fontSize: '1rem' }}>
          Clear Canvas
        </button>
      </div>
      <p style={{ fontSize: '1.2rem' }}>
        Last: {coordinates.length > 0 ? `(${coordinates.at(-1).x}, ${coordinates.at(-1).y})` : 'Waiting...'}
      </p>
    </div>
  );
}
