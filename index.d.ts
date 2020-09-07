declare module 'electron_share_memery' {
  export function SetShareMemery(string, number, ArrayBuffer): void;
  export function GetShareMemery(string): any;
  export function ClearShareMemery(): void;
}
