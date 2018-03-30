using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindGenCalc
{
    class Program
    {
        static void Main(string[] args)
        {
            //_Func1();

            double _rays = 50;
            double _diam = 4000, _rad = _diam / 2;

            double _rayang = 2 * Math.PI / _rays;
            double _circlength = Math.PI * _diam;
            double _rrc_width = Math.Sin(_rayang), _rrc_height = 1 - Math.Cos(_rayang);

            double _rayarclength = _circlength / _rays;
            double _horda = Math.Sqrt(Math.Pow(_rrc_width, 2) + Math.Pow(_rrc_height, 2)) * _rad;

            Console.WriteLine("Диаметр:          {0,-20} мм", _diam);
            Console.WriteLine("Радиус:           {0,-20} мм", _rad);
            Console.WriteLine("Длина окружности: {0,-20} мм", _circlength);
            Console.WriteLine("Количество лучей: {0,-20} шт", _rays);
            Console.WriteLine("Угол луча:        {0,-20} грд", _rayang / Math.PI * 180);
            Console.WriteLine();

            Console.WriteLine("Арка:             {0,-20} мм", _rayarclength);
            Console.WriteLine("Хорда:            {0,-20} мм", _horda);
            Console.WriteLine();

        }

        static void _Func1()
        {
            for (int _i = 1; _i < 256; _i++)
            {
                double _x = 9000.0 / _i;
                if (_x == Math.Round(_x)) Console.WriteLine("    WGT_{0}PPR = {1},", _x, _i);
            }
        }
    }
}
