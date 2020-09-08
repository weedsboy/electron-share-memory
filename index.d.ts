declare module 'electron_share_memory' {
  export function SetShareMemory(string, number, ArrayBuffer): void;
  export function GetShareMemory(string): any;
  export function ClearShareMemory(): void;
}
