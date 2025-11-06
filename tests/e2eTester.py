import sys
import os
import subprocess
import glob

def run_program(program_path, input_file, output_file):
    with open(input_file, 'r') as f:
        input_data = f.read()
    
    result = subprocess.run(
        [program_path],
        input=input_data,
        text=True,
        capture_output=True,
        timeout=5
    )
    
    if output_file:
        with open(output_file, 'r') as f:
            expected_output = f.read().strip()
        
        actual_output = result.stdout.strip()
        return actual_output == expected_output, actual_output, expected_output


def main():
    if len(sys.argv) != 3:
        print("Usage: python3 e2eTester.py <program_path> <test_cases_dir>")
        sys.exit(1)
    
    program_path = sys.argv[1]
    test_dir = sys.argv[2]
    
    # Find all .in files
    test_files = glob.glob(os.path.join(test_dir, "*.in"))
    test_files.sort()  # Sort for consistent order
    
    if not test_files:
        print(f"No test files found in {test_dir}")
        sys.exit(1)
    
    all_passed = True
    
    for test_in in test_files:
        test_name = os.path.basename(test_in).replace('.in', '')
        test_out = test_in.replace('.in', '.out')
        
        print(f"Running {test_name}...", end=' ')
        
        if not os.path.exists(test_out):
            print("SKIP (no .out file)")
            continue
        
        passed, actual, expected = run_program(program_path, test_in, test_out)
        
        if passed:
            print(f'PASSED {test_name}')
        else:
            print("FAILED")
            print(f"   Expected: {expected}")
            print(f"   Got:      {actual}")
            all_passed = False
    
    sys.exit(0 if all_passed else 1)

if __name__ == "__main__":
    main()