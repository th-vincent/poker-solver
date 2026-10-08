const DATA_URL = "../data/processed/qsjh2h_example.json";
const SUITS = { s: "♠", h: "♥", d: "♦", c: "♣" };
const stats = { total: 0, good: 0, ok: 0, mistake: 0 };
let data;
let current;

const $ = (id) => document.getElementById(id);

function cardsHTML(str) {
  return str.match(/.{2}/g).map((c) => {
    const red = c[1] === "h" || c[1] === "d";
    return `<span class="card ${red ? "red" : ""}">${c[0]}${SUITS[c[1]]}</span>`;
  }).join("");
}

function pick(arr) {
  return arr[Math.floor(Math.random() * arr.length)];
}

function parseAction(a) {
  const [type, amount] = a.split(" ");
  return { type, amount: amount ? parseFloat(amount) : null };
}

function fmtAmount(n) {
  return Number.isInteger(n) ? String(n) : n.toFixed(1);
}

function label(a) {
  const { type, amount } = parseAction(a);
  switch (type) {
    case "CHECK": return "Check";
    case "CALL": return "Call";
    case "FOLD": return "Fold";
    case "BET": return `Mise ${fmtAmount(amount)}`;
    case "RAISE": return `Relance à ${fmtAmount(amount)}`;
    default: return a;
  }
}

// Reconstitue le pot et le montant à payer à partir du chemin d'actions.
// Hypothèse : les montants du solveur sont des totaux misés sur la rue.
function spotState(path) {
  const steps = path === "root" ? [] : path.split("/");
  const invested = [0, 0]; // 0 = premier joueur à agir, 1 = l'autre
  steps.forEach((a, i) => {
    const who = i % 2;
    const { type, amount } = parseAction(a);
    if (type === "BET" || type === "RAISE") invested[who] = amount;
    else if (type === "CALL") invested[who] = invested[1 - who];
  });
  const me = steps.length % 2;
  return {
    steps,
    pot: data.pot + invested[0] + invested[1],
    toCall: Math.max(invested[0], invested[1]) - invested[me],
    stackLeft: data.stack - invested[me],
  };
}

function describeHistory(steps) {
  if (steps.length === 0) return "Aucune action avant toi.";
  const first = data.nodes.root.position;
  const second = first === "OOP" ? "IP" : "OOP";
  return steps.map((a, i) => `${i % 2 === 0 ? first : second} : ${label(a)}`).join(" → ");
}

// Barème de la v1 (convention du projet, pas une règle du solveur)
function grade(freq) {
  if (freq >= 0.30) return ["good", "Bonne réponse"];
  if (freq >= 0.05) return ["ok", "Acceptable (action mixte)"];
  return ["mistake", "Erreur"];
}

function newSpot() {
  const path = pick(Object.keys(data.nodes));
  const node = data.nodes[path];
  const hand = pick(Object.keys(node.hands));
  current = { path, node, hand };
  const s = spotState(path);

  $("board").innerHTML = cardsHTML(data.board);
  $("context").textContent =
    `Tu es ${node.position}. Pot : ${fmtAmount(s.pot)}. ` +
    (s.toCall > 0 ? `À payer : ${fmtAmount(s.toCall)}. ` : "Rien à payer. ") +
    `Stack restant : ${fmtAmount(s.stackLeft)}. Historique : ${describeHistory(s.steps)}`;
  $("hand").innerHTML = cardsHTML(hand);
  $("result").innerHTML = "";
  $("next").hidden = true;

  $("actions").innerHTML = "";
  node.actions.forEach((action, i) => {
    const b = document.createElement("button");
    b.textContent = label(action);
    b.addEventListener("click", () => answer(i));
    $("actions").appendChild(b);
  });
}

function answer(i) {
  const { node, hand } = current;
  const freqs = node.hands[hand];
  const [cls, lbl] = grade(freqs[i]);

  stats.total++;
  stats[cls]++;

  $("actions").querySelectorAll("button").forEach((b) => (b.disabled = true));
  const lines = node.actions.map((a, j) =>
    `<div class="${j === i ? "chosen" : ""}">${label(a)} : ${(freqs[j] * 100).toFixed(1)} %${j === i ? " ← ton choix" : ""}</div>`
  ).join("");
  $("result").innerHTML = `<p class="${cls}">${lbl}</p>${lines}`;
  $("next").hidden = false;
  $("stats").textContent =
    `Score : ${stats.good} bonnes, ${stats.ok} acceptables, ${stats.mistake} erreurs sur ${stats.total}`;
}

$("next").addEventListener("click", newSpot);

fetch(DATA_URL)
  .then((r) => r.json())
  .then((d) => { data = d; newSpot(); })
  .catch((e) => { $("context").textContent = `Erreur de chargement : ${e}`; });