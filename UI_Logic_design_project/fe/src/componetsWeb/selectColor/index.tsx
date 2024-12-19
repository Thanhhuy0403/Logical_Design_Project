"use client";

import { ChangeEvent, useState } from "react";

const ColorPicker = ({ onColorChange }: { onColorChange: (rgbArray: number[]) => void }) => {
    const [color, setColor] = useState("#ffffff");

    const hexToRgb = (hex: string): [number, number, number] => {
        const r = parseInt(hex.slice(1, 3), 16);
        const g = parseInt(hex.slice(3, 5), 16);
        const b = parseInt(hex.slice(5, 7), 16);
        return [r, g, b];
    };

    const handleColorChange = (e: ChangeEvent<HTMLInputElement>) => {
        const newColor = e.target.value;
        setColor(newColor);
        const rgbArray = hexToRgb(newColor);
        onColorChange(rgbArray);
    };

    return (
        <div
            className="flex flex-col items-center justify-center shadow-lg w-96 rounded-3xl h-[400px]"
            style={{
                background: "linear-gradient(135deg, #e3f2fd, #bbdefb)",
                borderRadius: "20px",
            }}
        >
            <p
                className="text-xl font-semibold mb-4 pt-6 pb-6 tracking-wide pl-5 pr-5"
                style={{
                    color: "#333",
                    fontFamily: "'Poppins', sans-serif",
                }}
            >
                🎨 Chọn màu yêu thích
            </p>

            <div
                className="relative flex items-center justify-center pt-8"
                style={{
                    width: "150px",
                    height: "150px",
                    marginBottom: "30px",
                }}
            >
                <input
                    type="color"
                    value={color}
                    onChange={handleColorChange}
                    style={{
                        position: "absolute",
                        width: "150px",
                        height: "150px",
                        opacity: 0,
                        cursor: "pointer",
                        zIndex: 2,
                    }}
                />
                <div
                    className="rounded-full shadow-lg pb-20"
                    style={{
                        width: "150px",
                        height: "150px",
                        background: color,
                        border: "1px solid #fff",
                        boxShadow: "0 8px 20px rgba(0, 0, 0, 0.2)",
                        transition: "transform 0.3s ease, box-shadow 0.3s ease",
                    }}
                ></div>
            </div>
        </div>
    );
};

export default ColorPicker;
