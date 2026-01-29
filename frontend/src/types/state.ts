// Application state types

export type FurnaceModeCode =
  | 0 // OFF
  | 1 // ERROR
  | 2 // PROFILE
  | 3; // MANUAL

export type ProfileSubStateCode =
  | 0 // NONE
  | 1 // LOADED
  | 2 // RUNNING
  | 3 // STOPPED
  | 4; // COMPLETED

export type ManualSubStateCode =
  | 0 // OFF
  | 1; // ON

export interface FurnaceState {
  mode: FurnaceModeCode;
  profile_state: ProfileSubStateCode;
  manual_state: ManualSubStateCode;
  program_name: string | null;
  kiln_temp: number;
  set_temp: number;
  env_temp: number;
  case_temp: number;
  heat_percent: number;
  temp_change: number;
  step: string;
  prog_start: string | null;
  prog_end: string | null;
  curr_time: string;
  error_message: string | null;
}

export interface EditorState {
  filename: string;
  isNew: boolean;
}

export interface ChartMarker {
  x: number;
  type: string;
  value?: string | number;
}

export interface ProgramProfile {
  name: string;
  startTime: number | null;
  durationMinutes: number;
  times: number[];
  temps: number[];
}

export interface ChartData {
  timestamps: number[]; // Unix seconds
  kilnTemps: number[];
  setTemps: number[];
  envTemps: number[];
  caseTemps: number[];
  markers: ChartMarker[];
}

export type PreferencesMap = Record<string, string>;

export interface StateMessage {
  type: 'state';
  data: FurnaceState;
}

export interface GenericMessage {
  type: string;
  data?: unknown;
}

export type IncomingMessage = StateMessage | GenericMessage;

export const MODE_NAMES: Record<number, string> = {
  0: 'OFF',
  1: 'ERROR',
  2: 'PROFILE',
  3: 'MANUAL',
};

export const PROFILE_STATE_NAMES: Record<number, string> = {
  0: 'NONE',
  1: 'LOADED',
  2: 'RUNNING',
  3: 'STOPPED',
  4: 'COMPLETED',
};

export const MANUAL_STATE_NAMES: Record<number, string> = {
  0: 'OFF',
  1: 'ON',
};

export function formatModeStatus(state: FurnaceState): string {
  switch (state.mode) {
    case 0:
      return 'OFF';
    case 1:
      return 'ERROR';
    case 2:
      return `PROFILE: ${PROFILE_STATE_NAMES[state.profile_state] || 'UNKNOWN'}`;
    case 3:
      return `MANUAL: ${MANUAL_STATE_NAMES[state.manual_state] || 'UNKNOWN'}`;
    default:
      return 'UNKNOWN';
  }
}

export function statusClass(state: FurnaceState): string {
  if (state.mode === 1) return 'error';
  if (state.mode === 2 && state.profile_state === 2) return 'running';
  return '';
}

