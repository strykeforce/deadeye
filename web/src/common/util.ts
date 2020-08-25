import { Camera, Units } from "./models";

export const messageOffset = "11vh";

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const get = (p: string[]) => (o: any) =>
  p.reduce((xs, x) => (xs && xs[x] ? xs[x] : null), o);

export const getCamera = (id: string, units: Units): Camera => {
  const u = id.charAt(0);
  const c = id.charAt(1);
  return get([u, "cameras", c])(units);
};

export const getIds = (units?: Units) => {
  if (units) {
    return Object.values(units)
      .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
      .sort();
  }
  return [];
};

export const key = (unit: string, inum: number, id: number) =>
  `${unit}${inum}${id}`;
