// Command sending functions

import { ws, state, setProgramProfile, setProgramProfileLocked } from './state.js';
import { log } from './views/debug.js';
import { getErrorMessage } from './utils.js';
import { updateChartData, setDefaultView } from './chart/dashboard.js';
import {
  encodeStartCommand,
  encodeStopCommand,
  encodeLoadCommand,
  encodeUnloadCommand,
  encodeSetTempCommand,
  encodeSetModeCommand,
} from './flatbuffers.js';

export function sendCommand(action: string, params: Record<string, unknown> = {}) {
  if (!ws || ws.readyState !== WebSocket.OPEN) {
    log('error', 'Not connected');
    return;
  }
  
  let encoded: Uint8Array | null = null;
  
  switch (action) {
    case 'start':
      encoded = encodeStartCommand(
        params.segment as number | undefined,
        params.minute as number | undefined
      );
      break;
    case 'stop':
      encoded = encodeStopCommand();
      break;
    case 'load':
      encoded = encodeLoadCommand(params.program as string);
      break;
    case 'unload':
      encoded = encodeUnloadCommand();
      break;
    case 'set_temp':
      encoded = encodeSetTempCommand(params.temperature as number);
      break;
    case 'set_mode':
      encoded = encodeSetModeCommand(params.mode as number);
      break;
    default:
      log('error', `Unknown command: ${action}`);
      return;
  }
  
  if (encoded) {
    log('sent', `[FlatBuffers] ${action}`);
    ws.send(encoded);
  }
}

export function loadProgram() {
  const select = document.getElementById('programSelect') as HTMLSelectElement | null;
  if (!select) {
    log('error', 'Program select not found');
    return;
  }
  const program = select.value;
  if (!program) {
    log('error', 'Select a program first');
    return;
  }
  sendCommand('load', { program });
}

export function clearProgram() {
  if (state.mode === 2 && state.profile_state === 2) {
    window.alert('Cannot unload while program is running. Stop the program first.');
    return;
  }
  sendCommand('unload');
  setProgramProfile(null);
  setProgramProfileLocked(false);
  const select = document.getElementById('programSelect') as HTMLSelectElement | null;
  if (select) select.value = '';
  updateChartData();
  setDefaultView();
}

export function setTemperature() {
  if (state.mode !== 3) {
    window.alert('Switch to Manual mode before setting temperature.');
    return;
  }
  const input = document.getElementById('tempInput') as HTMLInputElement | null;
  const temp = input ? parseFloat(input.value) : NaN;
  if (Number.isNaN(temp)) {
    log('error', 'Invalid temperature');
    return;
  }
  sendCommand('set_temp', { temperature: temp });
}

export function setMode() {
  const select = document.getElementById('modeSelect') as HTMLSelectElement | null;
  if (!select) {
    log('error', 'Mode select not found');
    return;
  }
  const value = select.value;
  const modeMap: Record<string, number> = { off: 0, error: 1, profile: 2, manual: 3 };
  const mode = modeMap[value];
  if (mode === undefined) {
    log('error', `Unknown mode: ${value}`);
    return;
  }
  sendCommand('set_mode', { mode });
}

export async function reboot() {
  if (!window.confirm('Reboot the device?')) return;
  log('sent', 'POST /api/reboot');
  try {
    const res = await window.fetch('/api/reboot', { method: 'POST' });
    const data = await res.json();
    log('ack', JSON.stringify(data));
  } catch (err) {
    log('error', getErrorMessage(err));
  }
}

