namespace UnityToUeConverter
{
    public partial class Form1 : Form
    {
        // Элементы интерфейса
        private TextBox txtUnityX, txtUnityY, txtUnityZ;
        private TextBox txtUnityRotX, txtUnityRotY, txtUnityRotZ, txtUnityRotW;
        private TextBox txtUeX, txtUeY, txtUeZ;
        private TextBox txtUeRotX, txtUeRotY, txtUeRotZ, txtUeRotW;
        private Button btnConvert;
        private Label lblPosIn, lblRotIn, lblPosOut, lblRotOut;

        public Form1()
        {
            InitializeComponentLayout();
        }

        private void InitializeComponentLayout()
        {
            this.Text = "Unity to UE Coordinate Converter";
            this.Width = 500;
            this.Height = 400;
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;

            // Входные данные: Позиция Unity (метры)
            lblPosIn = new Label() { Text = "Unity Position (X, Y, Z в метрах):", Left = 20, Top = 20, Width = 220 };
            txtUnityX = new TextBox() { Left = 20, Top = 45, Width = 60, Text = "1.0" };
            txtUnityY = new TextBox() { Left = 90, Top = 45, Width = 60, Text = "2.0" };
            txtUnityZ = new TextBox() { Left = 160, Top = 45, Width = 60, Text = "3.0" };

            // Входные данные: Кватернион Unity (X, Y, Z, W)
            lblRotIn = new Label() { Text = "Unity Rotation Quaternion (X, Y, Z, W):", Left = 20, Top = 90, Width = 250 };
            txtUnityRotX = new TextBox() { Left = 20, Top = 115, Width = 50, Text = "0.0" };
            txtUnityRotY = new TextBox() { Left = 80, Top = 115, Width = 50, Text = "0.0" };
            txtUnityRotZ = new TextBox() { Left = 140, Top = 115, Width = 50, Text = "0.0" };
            txtUnityRotW = new TextBox() { Left = 200, Top = 115, Width = 50, Text = "1.0" };

            // Кнопка конвертации
            btnConvert = new Button() { Text = "Конвертировать ➡️", Left = 20, Top = 160, Width = 230, Height = 30 };
            btnConvert.Click += BtnConvert_Click;

            // Выходные данные: Позиция UE (сантиметры)
            lblPosOut = new Label() { Text = "Unreal Engine Position (X, Y, Z в см):", Left = 20, Top = 210, Width = 250 };
            txtUeX = new TextBox() { Left = 20, Top = 235, Width = 70, ReadOnly = true };
            txtUeY = new TextBox() { Left = 100, Top = 235, Width = 70, ReadOnly = true };
            txtUeZ = new TextBox() { Left = 180, Top = 235, Width = 70, ReadOnly = true };

            // Выходные данные: Кватернион UE (X, Y, Z, W)
            lblRotOut = new Label() { Text = "Unreal Engine FQuat (X, Y, Z, W):", Left = 20, Top = 280, Width = 250 };
            txtUeRotX = new TextBox() { Left = 20, Top = 305, Width = 60, ReadOnly = true };
            txtUeRotY = new TextBox() { Left = 90, Top = 305, Width = 60, ReadOnly = true };
            txtUeRotZ = new TextBox() { Left = 160, Top = 305, Width = 60, ReadOnly = true };
            txtUeRotW = new TextBox() { Left = 230, Top = 305, Width = 60, ReadOnly = true };

            // Добавление элементов на форму
            this.Controls.AddRange(new Control[] {
                lblPosIn, txtUnityX, txtUnityY, txtUnityZ,
                lblRotIn, txtUnityRotX, txtUnityRotY, txtUnityRotZ, txtUnityRotW,
                btnConvert,
                lblPosOut, txtUeX, txtUeY, txtUeZ,
                lblRotOut, txtUeRotX, txtUeRotY, txtUeRotZ, txtUeRotW
            });
        }

        private void BtnConvert_Click(object sender, EventArgs e)
        {
            try
            {
                // Используем InvariantCulture для поддержки точки, а замена запятой на точку решает обе проблемы
                var culture = System.Globalization.CultureInfo.InvariantCulture;

                // Функция нормализации строки: заменяет запятую на точку и убирает лишние пробелы
                Func<string, string> normalize = (str) => str.Replace(',', '.').Trim();

                // 1. Чтение и валидация координат позиций
                double uniX = double.Parse(normalize(txtUnityX.Text), culture);
                double uniY = double.Parse(normalize(txtUnityY.Text), culture);
                double uniZ = double.Parse(normalize(txtUnityZ.Text), culture);

                // 2. Чтение и валидация компонентов кватерниона
                double uniRotX = double.Parse(normalize(txtUnityRotX.Text), culture);
                double uniRotY = double.Parse(normalize(txtUnityRotY.Text), culture);
                double uniRotZ = double.Parse(normalize(txtUnityRotZ.Text), culture);
                double uniRotW = double.Parse(normalize(txtUnityRotW.Text), culture);

                // 3. Трансформация позиции: свап осей и перевод метров в см
                double ueX = uniZ * 100.0;
                double ueY = uniX * 100.0;
                double ueZ = uniY * 100.0;

                // 4. Трансфикация вращения (кватерниона)
                double ueRotX = uniRotZ;
                double ueRotY = uniRotX;
                double ueRotZ = uniRotY;
                double ueRotW = uniRotW;

                // 5. Вывод результатов с инвариантной культурой (всегда будет выводить с точкой)
                txtUeX.Text = ueX.ToString("F4", culture);
                txtUeY.Text = ueY.ToString("F4", culture);
                txtUeZ.Text = ueZ.ToString("F4", culture);

                txtUeRotX.Text = ueRotX.ToString("F4", culture);
                txtUeRotY.Text = ueRotY.ToString("F4", culture);
                txtUeRotZ.Text = ueRotZ.ToString("F4", culture);
                txtUeRotW.Text = ueRotW.ToString("F4", culture);
            }
            catch (FormatException)
            {
                MessageBox.Show("Пожалуйста, введите корректные числа. Допускаются целые числа, а также разделители точка (.) или запятая (,).", "Ошибка ввода", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
