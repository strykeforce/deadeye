import { useState, useEffect } from 'react';

// eslint-disable-next-line @typescript-eslint/no-explicit-any
export const get = (p: string[]) => (o: any) => p.reduce((xs, x) => (xs && xs[x] ? xs[x] : null), o);

// from: https://dev.to/gabe_ragland/debouncing-with-react-hooks-jci
export const useDebounce = <T>(value: T, delay: number): T => {
  const [debouncedValue, setDebouncedValue] = useState(value);

  useEffect(() => {
    const handler = setTimeout(() => {
      setDebouncedValue(value);
    }, delay);

    return () => {
      clearTimeout(handler);
    };
  }, [value, delay]);

  return debouncedValue;
};
