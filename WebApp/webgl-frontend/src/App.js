import logo from "./logo.svg";
import "./App.css";
import CustomCanvas from "./components/CustomCanvas";
import { useState } from "react";
import { ChromePicker } from "react-color";

function App() {
  const [color, setColor] = useState("");
  const [isColorSelected, setColorSelectionValidation] = useState(false);

  const handleChangeComplete = (color) => {
    setColor(color.hex);
    console.log("Color", color);
  };

  const confirmColor = () => {
    if (color !== "") {
      setColorSelectionValidation(true);
    }
  };

  return (
    <div className="App">
      {!isColorSelected && (
        <ChromePicker color={color} onChangeComplete={handleChangeComplete} />
      )}
      {!isColorSelected && <button onClick={confirmColor}>Select</button>}
      {isColorSelected && <CustomCanvas objectColor={color}></CustomCanvas>}
    </div>
  );
}

export default App;
