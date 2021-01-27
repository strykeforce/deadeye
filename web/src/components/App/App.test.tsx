import React from "react";
import { render } from "@testing-library/react";
import { screen } from "@testing-library/dom";
import App from ".";

test("renders default page", () => {
  render(<App />);
  const element = screen.getByTestId("default-page");
  expect(element).toBeInTheDocument();
});
