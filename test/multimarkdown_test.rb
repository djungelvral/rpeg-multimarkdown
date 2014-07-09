$: << File.join(File.dirname(__FILE__), "../lib")

require 'minitest'
require 'multimarkdown'

MARKDOWN_TEST_DIR = "#{File.dirname(__FILE__)}/MultiMarkdownTest"

class MultiMarkdownTest < MiniTest::Unit::TestCase

  def test_that_extension_methods_are_present_on_multimarkdown_class
    assert MultiMarkdown.instance_methods.include?('to_html'),
      "MultiMarkdown class should respond to #to_html"
  end

  def test_that_simple_one_liner_goes_to_html
    multimarkdown = MultiMarkdown.new('Hello World.')
    assert_respond_to multimarkdown, :to_html
    assert_equal "<p>Hello World.</p>", multimarkdown.to_html.strip
  end

  def test_that_inline_multimarkdown_goes_to_html
    multimarkdown = MultiMarkdown.new('_Hello World_!')
    assert_respond_to multimarkdown, :to_html
    assert_equal "<p><em>Hello World</em>!</p>", multimarkdown.to_html.strip
  end

  def test_that_multimarkdown_in_html_goes_to_html
    multimarkdown = MultiMarkdown.new('Hello <span>_World_</span>!',:process_html)
    assert_respond_to multimarkdown, :to_html
    assert_equal "<p>Hello <span><em>World</em></span>!</p>", multimarkdown.to_html.strip
  end

  def test_that_bluecloth_restrictions_are_supported
    multimarkdown = MultiMarkdown.new('Hello World.')
    [:filter_html, :filter_styles].each do |restriction|
      assert_respond_to multimarkdown, restriction
      assert_respond_to multimarkdown, "#{restriction}="
    end
    assert_not_equal true, multimarkdown.filter_html
    assert_not_equal true, multimarkdown.filter_styles

    multimarkdown = MultiMarkdown.new('Hello World.', :filter_html, :filter_styles)
    assert_equal true, multimarkdown.filter_html
    assert_equal true, multimarkdown.filter_styles
  end

  def test_that_redcloth_attributes_are_supported
    multimarkdown = MultiMarkdown.new('Hello World.')
    assert_respond_to multimarkdown, :fold_lines
    assert_respond_to multimarkdown, :fold_lines=
    assert_not_equal true, multimarkdown.fold_lines

    multimarkdown = MultiMarkdown.new('Hello World.', :fold_lines)
    assert_equal true, multimarkdown.fold_lines
  end

  def test_that_redcloth_to_html_with_single_arg_is_supported
    multimarkdown = MultiMarkdown.new('Hello World.')
    assert_nothing_raised(ArgumentError) { multimarkdown.to_html(true) }
  end



  # Build tests for each file in the MarkdownTest test suite
  ["Tests","MultiMarkdownTests","BeamerTests","MemoirTests"].each do |subdir|

    Dir["#{MARKDOWN_TEST_DIR}/#{subdir}/*.text"].each do |text_file|

      basename = File.basename(text_file).sub(/\.text$/, '')
      html_file = text_file.sub(/text$/, 'html')
      method_name = basename.gsub(/[-,]/, '').gsub(/\s+/, '_').downcase

      define_method "test_#{method_name}" do
        multimarkdown = MultiMarkdown.new(File.read(text_file),:compatibility)
        actual_html = multimarkdown.to_html
        assert_not_nil actual_html
      end

      define_method "test_#{method_name}_with_smarty_enabled" do
        multimarkdown = MultiMarkdown.new(File.read(text_file), :smart)
        actual_html = multimarkdown.to_html
        assert_not_nil actual_html
      end

    end

  end

end
