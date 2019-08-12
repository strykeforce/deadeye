import { useState, useEffect } from 'react';

export const get = p => o => p.reduce((xs, x) => (xs && xs[x] ? xs[x] : null), o);

// from: https://dev.to/gabe_ragland/debouncing-with-react-hooks-jci
export const useDebounce = (value, delay) => {
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
