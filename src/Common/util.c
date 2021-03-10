int all_ele_true(int *array, int size)
 {
     int is_true = 1;
     for (int i = 0; i < size; i++)
     {
         if (array[i] != 1)
         {
             is_true = 0;
             break;
         }
     }
     return is_true;
 }
