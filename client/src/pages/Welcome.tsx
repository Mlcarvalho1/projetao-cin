import { Link } from 'react-router-dom';

export function Welcome() {
  return (
    <div className="container" style={{ paddingTop: '6rem' }}>
      <span className="tag">Projetão CIn · MVP</span>
      <h1>
        Aqua<span style={{ color: 'var(--accent)' }}>Sense</span>
        <br />
        Irrigação inteligente no semiárido.
      </h1>
      <p className="lead">
        Sensores de umidade no campo, alertas no WhatsApp e recomendações
        prescritivas para o produtor médio do Vale do São Francisco.
      </p>
      <div style={{ display: 'flex', gap: '1rem', flexWrap: 'wrap' }}>
        <Link to="/dashboard" className="btn btn-primary">
          Abrir Dashboard
        </Link>
        <a href="/lps/index.html" className="btn btn-outline">
          Ver apresentação
        </a>
      </div>
    </div>
  );
}
