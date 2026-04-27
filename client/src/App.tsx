import { BrowserRouter, NavLink, Route, Routes } from 'react-router-dom';

import { Dashboard } from './pages/Dashboard';
import { Welcome } from './pages/Welcome';

export default function App() {
  return (
    <BrowserRouter>
      <nav className="topbar">
        <NavLink to="/" className="brand">
          Aqua<span>Sense</span>
        </NavLink>
        <div>
          <NavLink to="/" end>
            Welcome
          </NavLink>
          <NavLink to="/dashboard">Dashboard</NavLink>
        </div>
      </nav>
      <Routes>
        <Route path="/" element={<Welcome />} />
        <Route path="/dashboard" element={<Dashboard />} />
        <Route
          path="*"
          element={
            <div className="container">
              <h1>404</h1>
              <p className="lead">Página não encontrada.</p>
            </div>
          }
        />
      </Routes>
    </BrowserRouter>
  );
}
