using OfficeOpenXml;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MockProjectInterface
{
    public partial class Delete : Form
    {
        public Delete()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var package = new ExcelPackage(new System.IO.FileInfo(@"..\..\ImportData.xlsx"));

            ExcelWorksheet worksheet = package.Workbook.Worksheets[1];
            int j = 1;

            for (int i = worksheet.Dimension.Start.Row + 1; i <= worksheet.Dimension.End.Row; i++)
            {
                if (worksheet.Cells[i, 1].Value != null && txbID.Text == worksheet.Cells[i, 1].Value.ToString())
                {
                   
                    worksheet.Cells[i, j++].Clear();
                    worksheet.Cells[i, j++].Clear();
                    worksheet.Cells[i, j++].Clear();
                    worksheet.Cells[i, j++].Clear();
                    worksheet.Cells[i, j++].Clear();
                    break;
                }
            }
            package.Save();
            Close();
        }
    }
}
