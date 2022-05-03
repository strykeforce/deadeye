
export const messageOffset = "60px";

export const get = (p) => (o) =>
  p.reduce((xs, x) => (xs && xs[x] ? xs[x] : null), o);

export const getCamera = (id, units) => {
  const u = id.charAt(0);
  const c = id.charAt(1);
  return get([u, "cameras", c])(units);
};

export const getIds = (units) => {
  if (units) {
    return Object.values(units)
      .flatMap((u) => Object.values(u.cameras).map((c) => c.id))
      .sort();
  }
  return [];
};

export const key = (unit, inum, id) =>
  `${unit}${inum}${id}`;
