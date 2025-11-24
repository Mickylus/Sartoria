// Script per caricare e visualizzare l'inventario dei tessuti (Inventario.txt).
// Contesto: questo file si trova in `web/` e quando si serve la cartella `Codice`
// il file sorgente `Inventario.txt` è nella cartella padre (`../Inventario.txt`).

/**
 * loadInventory
 * - Legge il file `Inventario.txt` via fetch, lo suddivide in righe e ritorna
 *   un array di oggetti che rappresentano i singoli tessuti.
 * - Nota: la prima riga del file è metadata (es. numero di elementi) e viene scartata.
 * - Il parsing dei campi è "best-effort": il file sembra essere separato da
 *   spazi e contiene sia token testuali (nome, categoria, colore, ...) che
 *   numerici (quantità, larghezza, prezzi, data). Se il formato cambia,
 *   bisognerà aggiornare questa mappatura.
 */
async function loadInventory() {
  // Percorso relativo: dal file `web/script.js` si sale di una cartella
  // per leggere `Codice/Inventario.txt` quando il server è impostato su `Codice`.
  // Aggiungo un parametro timestamp per evitare cache del browser
  const url = `../Inventario.txt?t=${Date.now()}`;
  debug(`fetch: ${url}`);
  const resp = await fetch(url);
  debug(`fetch status: ${resp.status} ${resp.statusText} -> ${resp.url}`);
  if (!resp.ok) {
    debug(`Errore fetch Inventario: ${resp.status} ${resp.statusText} (${resp.url})`);
    throw new Error('Impossibile caricare Inventario.txt: ' + resp.status);
  }
  const txt = await resp.text();
  debug(`loadInventory: letti ${txt.split(/\r?\n/).length} righe`);
  // Provo ad ottenere l'intestazione Last-Modified per verificare il file sul server
  try {
    const headResp = await fetch(`../Inventario.txt?t=${Date.now()}`, { method: 'HEAD' });
    const lm = headResp.headers.get('Last-Modified') || headResp.headers.get('last-modified');
    if (lm) debug('Inventario.txt Last-Modified: ' + lm);
    // Aggiorna indicatore visibile nella pagina
    try {
      const el = document.getElementById('fetch-status');
      if (el) el.textContent = `Ultimo Aggiornamento: ${new Date().toLocaleString()}`;
    } catch(e){}
  } catch(e){ debug('No Last-Modified header: ' + e); }

  // Divido il testo in righe, rimuovo eventuali righe vuote
  const lines = txt.trim().split(/\r?\n/).filter(Boolean);
  if (lines.length === 0) return [];

  // Scarto la prima riga (metadata) e considero ogni riga successiva come un elemento
  const dataLines = lines.slice(1);
  // Esponi le righe raw per debug/associazione con la tabella
  try { window.__lastRawLines = dataLines.slice(); } catch(e){}

  // Mappatura dei token: adattare se il formato cambia
  const items = dataLines.map(line => {
    // per debug: logga la riga attuale (ma non troppo verbose)
    // console.debug('parsing line:', line);
    const tokens = line.split(/\s+/);
    const name = tokens[0] || '';        // es. Sintetico_Giallo
    const category = tokens[1] || '';    // es. TessutiSintetici
    const material = tokens[2] || '';    // es. Sintetico
    const color = tokens[3] || '';       // es. Giallo
    const pattern = tokens[4] || '';     // es. Nessuna / Righe
    const quantity = parseFloat(tokens[5]) || 0; // quantità (es. metri)
    const width = parseFloat(tokens[6]) || 0;    // larghezza o altro valore numerico
    const code = tokens[7] || '';        // codice stringa (es. Yellow80)
    // Il prezzo è stato mappato qui come token[13] (best-effort). Se hai
    // un significato preciso per le colonne, comunicamelo e aggiorno il mapping.
    const price = parseFloat(tokens[13]) || 0;

    // La data sembra trovarsi in tre token separati (giorno mese anno)
    let date = '';
    if (tokens[10] && tokens[11] && tokens[12]) {
      // Uso padStart per garantire 2 cifre per giorno/mese
      date = `${tokens[10].padStart(2,'0')}/${tokens[11].padStart(2,'0')}/${tokens[12]}`;
    }

    return { name, category, material, color, pattern, quantity, width, code, price, date };
  });
  // keep items also global for debug
  try { window.__lastItems = items.slice(); } catch(e){}
  return items;
}

// Funzione pubblica per ricaricare dati senza ricaricare la pagina
async function reloadData() {
  try {
    const [items, projects] = await Promise.all([loadInventory(), loadProjects()]);
    console.debug('reloadData: items', items.length, 'projects', projects.length);
    renderInventory(items);
    renderProjects(projects, items);
  } catch (e) {
    console.error('reloadData error:', e);
    alert('Errore durante il reload: ' + e.message);
  }
}

// Enhanced reload that also fetches and shows raw files for debugging
async function reloadDataWithRaw() {
  try {
    const [items, projects] = await Promise.all([loadInventory(), loadProjects()]);
    renderInventory(items);
    renderProjects(projects, items);
    // fetch raw text and display
    try {
      const r1 = await fetch(`../Inventario.txt?t=${Date.now()}`);
      const t1 = await r1.text();
      debug('Raw Inventario.txt:\n' + t1);
    } catch(e){ debug('Errore fetching raw Inventario: '+e); }
    try {
      const r2 = await fetch(`../Progetti.txt?t=${Date.now()}`);
      const t2 = await r2.text();
      debug('Raw Progetti.txt:\n' + t2);
    } catch(e){ debug('Errore fetching raw Progetti: '+e); }
  } catch (e) {
    console.error('reloadDataWithRaw error:', e);
    alert('Errore durante il reload: ' + e.message);
  }
}


/**
 * loadProjects
 * - Legge `Progetti.txt` e costruisce un array di progetti. Ogni progetto ha
 *   proprietà: name, rawFields (header tokens) e components (array di {name, qty}).
 * - Formato atteso (best-effort):
 *   linea 0: numero di progetti
 *   per ogni progetto:
 *     header: <projectName> ... <numComponents> ...
 *     seguite da <numComponents> righe: <fabricName> <quantity>
 */
async function loadProjects() {
  const url = `../Progetti.txt?t=${Date.now()}`;
  debug(`fetch: ${url}`);
  const resp = await fetch(url);
  debug(`fetch status: ${resp.status} ${resp.statusText} -> ${resp.url}`);
  if (!resp.ok) {
    // Se il file non esiste, ritorno array vuoto invece di lanciare
    debug('Progetti.txt non trovato: ' + resp.status);
    return [];
  }
  const txt = await resp.text();
  debug(`loadProjects: letti ${txt.split(/\r?\n/).length} righe`);
  const lines = txt.trim().split(/\r?\n/).filter(Boolean);
  if (lines.length === 0) return [];

  // Skip primo numero (numero progetti) se presente
  let idx = 1;
  const projects = [];
  while (idx < lines.length) {
    const headerTokens = lines[idx].split(/\s+/);
    const projName = headerTokens[0] || `progetto_${projects.length+1}`;
    // Il numero di componenti sembra trovarsi token[5] (best-effort)
    const numComp = parseInt(headerTokens[5], 10) || 0;
    const rawFields = headerTokens;
    // Provo a leggere possibili valori numerici presenti nell'header del progetto
    // token[6] frequentemente è ricavo, token[7]/[8] possono contenere duplicati
    // o il costo; applichiamo una semplice heuristica per scegliere il valore
    // corretto evitando situazioni in cui token[7] == token[6].
    const t6 = parseFloat(headerTokens[6]);
    const t7 = parseFloat(headerTokens[7]);
    const t8 = parseFloat(headerTokens[8]);
    const projectRevenue = Number.isFinite(t6) && t6 > 0 ? t6 : (Number.isFinite(t7) && t7 > 0 ? t7 : (Number.isFinite(t8) ? t8 : 0));
    // Preferiamo token[8] come costo se presente e diverso dal ricavo.
    let projectCostFromFile = 0;
    if (Number.isFinite(t8) && t8 > 0 && t8 !== projectRevenue) projectCostFromFile = t8;
    else if (Number.isFinite(t7) && t7 > 0 && t7 !== projectRevenue) projectCostFromFile = t7;
    else projectCostFromFile = 0;
    const components = [];
    for (let j = 0; j < numComp; j++) {
      const compLine = lines[idx + 1 + j];
      if (!compLine) break;
      const t = compLine.split(/\s+/);
      const name = t[0] || '';
      const qty = parseFloat(t[1]) || 0;
      components.push({ name, qty });
    }
    projects.push({ name: projName, rawFields, components, revenue: projectRevenue, costFromFile: projectCostFromFile });
    idx += 1 + numComp;
  }
  return projects;
}


/**
 * renderInventory
 * - Riceve un array di oggetti e popola la tabella HTML (`#inventory tbody`).
 */
function renderInventory(items) {
  const tbody = document.querySelector('#inventory tbody');
  tbody.innerHTML = '';
  const rawLines = window.__lastRawLines || [];
  for (let i = 0; i < items.length; i++) {
    const it = items[i];
    const tr = document.createElement('tr');
    // escapeHtml viene usata per prevenire injection di HTML
    tr.innerHTML = `
      <td>${i+1}</td>
      <td>${escapeHtml(it.name)}</td>
      <td>${escapeHtml(it.category)}</td>
      <td>${escapeHtml(it.material)}</td>
      <td>${escapeHtml(it.color)}</td>
      <td>${escapeHtml(it.pattern)}</td>
      <td>${it.quantity}</td>
      <td>${it.width}</td>
      <td>${escapeHtml(it.code)}</td>
      <td>${it.price}</td>
      <td>${escapeHtml(it.date)}</td>
    `;
    tbody.appendChild(tr);
    // aggiungo riga raw (nascosta) per ogni elemento, utile per debug e per capire parsing
    const rawTr = document.createElement('tr');
    rawTr.className = 'raw-row';
    const rawTd = document.createElement('td');
    rawTd.colSpan = 11;
    const rawText = rawLines[i] || '';
    rawTd.innerHTML = `<small style="color:#666">${escapeHtml(rawText)}</small>`;
    rawTr.style.display = 'none';
    rawTr.appendChild(rawTd);
    tbody.appendChild(rawTr);
    // click su riga principale per toggle della raw
    tr.style.cursor = 'pointer';
    tr.addEventListener('click', () => {
      rawTr.style.display = rawTr.style.display === 'none' ? '' : 'none';
    });
  }
}

// Scrive messaggi nell'area di debug (e in console). Usa pre esistente `#debug-output`.
function debug(msg) {
  try {
    console.debug(msg);
    const el = document.getElementById('debug-output');
    if (el) {
      const now = new Date().toLocaleTimeString();
      el.textContent = now + ' - ' + msg + "\n" + el.textContent;
    }
  } catch (e) {
    console.debug('debug write failed', e);
  }
}


/**
 * escapeHtml
 * - Semplice funzione di utilità per sostituire i caratteri speciali HTML
 *   con le rispettive entità (evita problemi se i campi contengono simboli).
 */
function escapeHtml(s) {
  return (s+'').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":"&#39;"}[c]));
}


/**
 * setupSearch
 * - Collega l'input di ricerca (`#search`) per filtrare l'array di elementi
 *   usando confronto case-insensitive su `name`, `color`, `category`, `material`.
 */
function setupSearch(items) {
  const input = document.getElementById('search');
  if (input) {
    input.addEventListener('input', () => {
    const q = input.value.trim().toLowerCase();
    if (!q) return renderInventory(items);
    const filtered = items.filter(it =>
      it.name.toLowerCase().includes(q) ||
      it.color.toLowerCase().includes(q) ||
      it.category.toLowerCase().includes(q) ||
      it.material.toLowerCase().includes(q)
    );
    renderInventory(filtered);
  });
  }
}


// IIFE principale: carica dati, renderizza tabella e imposta la ricerca.
/**
 * renderProjects
 * - Mostra la lista dei progetti e, per ciascuno, la lista dei componenti
 *   richiesti. Fa anche una verifica semplice confrontando la quantità
 *   richiesta con la quantità disponibile in inventario (se presente).
 */
function renderProjects(projects, inventoryItems) {
  const root = document.getElementById('projects');
  root.innerHTML = '';
  if (!projects || projects.length === 0) {
    root.innerHTML = '<p>Nessun progetto trovato.</p>';
    return;
  }

  for (const p of projects) {
    const details = document.createElement('details');
    const summary = document.createElement('summary');

    // Preferiamo usare i valori forniti nel file Progetti.txt se presenti
    // (p.costFromFile e p.revenue). Se non sono presenti, cadiamo
    // indietro al calcolo a partire dai prezzi presenti in inventario.
    let projectCost = 0;
    let costSource = 'calcolato da inventario';
    if (p.costFromFile && p.costFromFile > 0) {
      projectCost = p.costFromFile;
      costSource = 'da Progetti.txt';
    } else {
      for (const c of p.components) {
        const inv = inventoryItems.find(it => it.name === c.name);
        const price = inv ? (parseFloat(inv.price) || 0) : 0;
        projectCost += (parseFloat(c.qty) || 0) * price;
      }
    }
    const projectRevenue = parseFloat(p.revenue) || 0;
    const projectProfit = projectRevenue - projectCost;

    summary.textContent = `${p.name} — componenti: ${p.components.length}`;
    const metaSpan = document.createElement('span');
    metaSpan.style.marginLeft = '10px';
    metaSpan.style.fontSize = '0.95rem';
    metaSpan.style.color = '#333';
    metaSpan.textContent = `Costo approssimato: ≈ €${projectCost.toFixed(2)} (${costSource}) — Ricavi: €${projectRevenue.toFixed(2)} — Profitto: €${projectProfit.toFixed(2)}`;
    summary.appendChild(metaSpan);
    details.appendChild(summary);

    const table = document.createElement('table');
    table.className = 'project-table';
    table.innerHTML = `
      <thead><tr><th>Tessuto</th><th>Quantità richiesta</th><th>Disponibile</th><th>Stato</th></tr></thead>
      <tbody></tbody>
    `;
    const tbody = table.querySelector('tbody');

    for (const c of p.components) {
      const inv = inventoryItems.find(it => it.name === c.name);
      const available = inv ? inv.quantity : 0;
      const ok = available >= c.qty;
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${escapeHtml(c.name)}</td>
        <td>${c.qty}</td>
        <td>${available}</td>
        <td>${ok ? 'OK' : 'Scarso'}</td>
      `;
      tbody.appendChild(tr);
    }

    // Riga riepilogo costi (sotto la tabella)
    const summaryDiv = document.createElement('div');
    summaryDiv.style.marginTop = '8px';
    summaryDiv.style.fontSize = '0.95rem';
    summaryDiv.innerHTML = `<strong>Costo approssimato:</strong> ≈ €${projectCost.toFixed(2)} (${costSource}) &nbsp; <strong>Ricavi:</strong> €${projectRevenue.toFixed(2)} &nbsp; <strong>Profitto:</strong> €${projectProfit.toFixed(2)}`;
    details.appendChild(table);
    details.appendChild(summaryDiv);
    root.appendChild(details);
  }
}


// IIFE principale: carica inventario e progetti, renderizza tabelle e imposta la ricerca.
(async function main(){
  try {
    const [items, projects] = await Promise.all([loadInventory(), loadProjects()]);
    // Esponi gli oggetti caricati globalmente per debug dalla Console
    try { window.__lastItems = items; window.__lastProjects = projects; } catch(e){}
    renderInventory(items);
    renderProjects(projects, items);
    setupSearch(items);
    // Mostra i nomi degli elementi letti nell'area debug per controllo rapido
    try {
      const names = items.map((it,i)=>`${i+1}. ${it.name}`);
      debug('inventory names:\n' + names.join('\n'));
    } catch(e){}
    // Connessione del pulsante di reload se presente
    const rb = document.getElementById('reload-btn');
    if (rb) {
      rb.addEventListener('click', () => reloadDataWithRaw());
    }
    // Auto-refresh setup
    const autoCb = document.getElementById('auto-refresh-checkbox');
    const autoSec = document.getElementById('auto-refresh-seconds');
    let autoTimer = null;
    function startAuto() {
      stopAuto();
      const s = Math.max(1, parseInt(autoSec.value, 10) || 5);
      autoTimer = setInterval(() => reloadDataWithRaw(), s * 1000);
      debug('Auto-refresh attivato ogni ' + s + 's');
    }
    function stopAuto() {
      if (autoTimer) { clearInterval(autoTimer); autoTimer = null; debug('Auto-refresh fermato'); }
    }
    if (autoCb) {
      autoCb.addEventListener('change', () => {
        if (autoCb.checked) startAuto(); else stopAuto();
      });
    }
    if (autoSec) autoSec.addEventListener('change', () => { if (autoCb && autoCb.checked) startAuto(); });
    debug('Pagina inizializzata');
  } catch (e) {
    const el = document.getElementById('debug-output');
    if (el) el.textContent = 'Errore inizializzazione: ' + e.message + '\n' + (el.textContent || '');
    console.error(e);
    // Mostro l'errore anche in pagina per visibilità
    document.body.insertAdjacentHTML('afterbegin', '<p class="error">Errore: ' + (e.message || e) + '</p>');
  }
})();
