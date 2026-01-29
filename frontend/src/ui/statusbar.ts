// Status bar and UI updates

import { state, isProgramRunning, setIsProgramRunning } from '../state.js';
import { formatModeStatus, statusClass } from '../types/state.js';
import { formatTemp } from '../utils.js';

export function updateUI() {
  const s = state;

  // Status bar
  const statusKiln = document.getElementById('statusKiln');
  const statusTarget = document.getElementById('statusTarget');
  const statusEnv = document.getElementById('statusEnv');
  const statusHeat = document.getElementById('statusHeat');
  if (statusKiln) statusKiln.textContent = formatTemp(s.kiln_temp);
  if (statusTarget) statusTarget.textContent = formatTemp(s.set_temp);
  if (statusEnv) statusEnv.textContent = formatTemp(s.env_temp);
  if (statusHeat) statusHeat.textContent = `${s.heat_percent || 0}%`;

  const badge = document.getElementById('statusBadge');
  if (badge) {
    badge.textContent = formatModeStatus(s);
    badge.className = 'program-status ' + statusClass(s);
  }

  // Dashboard
  const dashKiln = document.getElementById('dashKiln');
  const dashTarget = document.getElementById('dashTarget');
  const dashEnv = document.getElementById('dashEnv');
  const dashCase = document.getElementById('dashCase');
  const dashStatus = document.getElementById('dashStatus');
  const dashProgram = document.getElementById('dashProgram');
  const dashStep = document.getElementById('dashStep');
  const dashHeat = document.getElementById('dashHeat');
  const dashStart = document.getElementById('dashStart');
  const dashEnd = document.getElementById('dashEnd');
  const dashTime = document.getElementById('dashTime');
  const dashChange = document.getElementById('dashChange');

  if (dashKiln) dashKiln.textContent = formatTemp(s.kiln_temp);
  if (dashTarget) dashTarget.textContent = formatTemp(s.set_temp);
  if (dashEnv) dashEnv.textContent = formatTemp(s.env_temp);
  if (dashCase) dashCase.textContent = formatTemp(s.case_temp);
  if (dashStatus) dashStatus.textContent = formatModeStatus(s) || '--';
  if (dashProgram) dashProgram.textContent = s.program_name || '--';
  if (dashStep) dashStep.textContent = s.step || '--';
  if (dashHeat) dashHeat.textContent = `${s.heat_percent || 0}%`;
  if (dashStart) dashStart.textContent = s.prog_start || '--';
  if (dashEnd) dashEnd.textContent = s.prog_end || '--';
  if (dashTime) dashTime.textContent = s.curr_time || '--';
  if (dashChange) dashChange.textContent = `${(s.temp_change || 0).toFixed(1)}°C/h`;

  const isRunning = s.mode === 2 && s.profile_state === 2;
  updateLoadControls(isRunning);
  updateStartButton(isRunning);
  updateManualControls(s.mode === 3);
  updateModeSelector(s.mode);
}

export function updateLoadControls(isRunning: boolean) {
  setIsProgramRunning(isRunning);
  const select = document.getElementById('programSelect') as HTMLSelectElement | null;
  if (select) select.disabled = isRunning;
  const sidebarBtn = document.getElementById('sidebarLoadBtn') as HTMLButtonElement | null;
  if (sidebarBtn) sidebarBtn.disabled = isRunning;
  const clearBtn = document.getElementById('sidebarClearBtn') as HTMLButtonElement | null;
  if (clearBtn) clearBtn.disabled = isRunning;
  applyProgramLoadButtons();
}

export function applyProgramLoadButtons() {
  document.querySelectorAll<HTMLButtonElement>('.program-load-btn').forEach(btn => {
    btn.disabled = isProgramRunning;
    btn.classList.toggle('primary', !isProgramRunning);
    btn.classList.toggle('running-disabled', isProgramRunning);
  });
}

export function updateStartButton(isRunning: boolean) {
  const btn = document.getElementById('startBtn') as HTMLButtonElement | null;
  if (!btn) return;
  btn.disabled = isRunning;
  btn.classList.toggle('primary', !isRunning);
  btn.classList.toggle('running-disabled', isRunning);
}

export function updateManualControls(isManual: boolean) {
  const input = document.getElementById('tempInput') as HTMLInputElement | null;
  const btn = document.querySelector<HTMLButtonElement>('button[onclick="setTemperature()"]');
  if (input) input.disabled = !isManual;
  if (btn) btn.disabled = !isManual;
}

export function updateModeSelector(mode: number) {
  const select = document.getElementById('modeSelect') as HTMLSelectElement | null;
  if (!select) return;
  const valueMap: Record<number, string> = { 0: 'off', 1: 'error', 2: 'profile', 3: 'manual' };
  const newValue = valueMap[mode];
  if (newValue && select.value !== newValue) {
    select.value = newValue;
  }
}

